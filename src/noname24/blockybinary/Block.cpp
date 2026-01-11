/*     ./src/noname24/blockybinary/Block.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

#include <print> // std::print

namespace NoName24 {
    namespace BlockyBinary {
        size_t Block::get_selfsize() {
            size_t selfsize = 0;

            std::vector<uint8_t> dump_vector = dump();
            selfsize += dump_vector.size();

            return selfsize;
        }

        void Block::parse(std::span<const uint8_t> data) {
            size_t byte_shift = 0; // сдвиг

            // размер блока
            std::span<const uint8_t> block_size_span = data.subspan(byte_shift, 8);
            uint64_t block_size = IntHelper::uvec8_to_uint64(
                std::vector<uint8_t>(block_size_span.begin(), block_size_span.end())
            );
            byte_shift += 8;

            // base_settings
            BlockSettings settings;
            byte_shift += settings.parse(data.subspan(byte_shift));

            this->settings = settings;

            // name
            std::span<const uint8_t> name_size_span = data.subspan(byte_shift, 2);
            uint16_t name_size = IntHelper::uvec8_to_uint16(
                std::vector<uint8_t>(name_size_span.begin(), name_size_span.end())
            );
            byte_shift += 2;
            name.assign(reinterpret_cast<const char*>(data.data() + byte_shift), name_size);
            byte_shift += name_size;

            // данные
            std::span<const uint8_t> data_size_compressed_span = data.subspan(byte_shift, 8);
            uint64_t data_size_compressed = IntHelper::uvec8_to_uint64(
                std::vector<uint8_t>(data_size_compressed_span.begin(), data_size_compressed_span.end())
            );
            byte_shift += 8;

            std::span<const uint8_t> data_compressed = data.subspan(byte_shift, data_size_compressed);
            std::vector<uint8_t> data_uncompressed_storage;
            std::span<const uint8_t> data_uncompressed;
            if(settings.compression_type == 1) {
                BlockSettings_Deflate& settings_deflate = settings.compression_type_1;
                data_uncompressed_storage.resize(settings_deflate.expected_size);
                mz_ulong dest_len = settings_deflate.expected_size;
                int status = mz_uncompress(
                    data_uncompressed_storage.data(),
                    &dest_len,
                    data_compressed.data(),
                    data_compressed.size()
                );
                if(status != MZ_OK) {
                    throw std::runtime_error("Deflate uncompress failed");
                }
                data_uncompressed = std::span<const uint8_t>(data_uncompressed_storage.data(), dest_len);
            } else {
                data_uncompressed = data_compressed;
            }

            uint64_t data_size_uncompressed = data_uncompressed.size();

            size_t byte_shift_2 = 0;
            for(int i = 0; i < settings.block_number; i++) {
                std::span<const uint8_t> data_blocks_block_size_span = data_uncompressed.subspan(byte_shift_2, 8);
                uint64_t data_blocks_block_size = IntHelper::uvec8_to_uint64(
                    std::vector<uint8_t>(data_blocks_block_size_span.begin(), data_blocks_block_size_span.end())
                );

                std::span<const uint8_t> data_blocks_block_data = data_uncompressed.subspan(byte_shift_2, data_blocks_block_size);
                Block data_blocks_block;
                data_blocks_block.parse(data_blocks_block_data);

                data_blocks.push_back(data_blocks_block);

                byte_shift_2 += data_blocks_block_size;
                data_size_uncompressed -= data_blocks_block_size;
            }

            std::span<const uint8_t> data_main_span = data_uncompressed.subspan(byte_shift_2, data_size_uncompressed);
            data_main = std::vector<uint8_t>(data_main_span.begin(), data_main_span.end());

            byte_shift += data_size_compressed;

            // xxh3 / xxh128
            if(settings.xxh3_bit == 0) {
                std::span<const uint8_t> xxh3_span = data.subspan(byte_shift, XXH64_SIZE);
                xxh3 = IntHelper::uvec8_to_uint64(
                    std::vector<uint8_t>(xxh3_span.begin(), xxh3_span.end())
                );
                byte_shift += XXH64_SIZE;
            } else
            if(settings.xxh3_bit == 1) {
                std::span<const uint8_t> xxh128_span = data.subspan(byte_shift, XXH128_SIZE);
                xxh128.low64 = IntHelper::uvec8_to_uint64(
                    std::vector<uint8_t>(xxh128_span.begin(), xxh128_span.begin() + XXH128_SIZE / 2)
                );
                xxh128.high64 = IntHelper::uvec8_to_uint64(
                    std::vector<uint8_t>(xxh128_span.begin() + XXH128_SIZE / 2, xxh128_span.begin() + XXH128_SIZE)
                );
                byte_shift += XXH128_SIZE;
            }

            // Проверка целостности (сверка xxh3)
            if(settings.xxh3_bit == 0) {
                uint64_t xxh3_current = XXH3_64bits(data.data(), data.size() - XXH64_SIZE);
                if(xxh3_current != xxh3) {
                    throw std::runtime_error(name + " не прошел проверку на целостность (xxh3)");
                }
            } else
            if(settings.xxh3_bit == 1) {
                XXH128_hash_t xxh128_current = XXH3_128bits(data.data(), data.size() - XXH128_SIZE);
                if(xxh128_current.low64 != xxh128.low64) {
                    throw std::runtime_error(name + " не прошел проверку на целостность (xxh128 low64)");
                }
                if(xxh128_current.high64 != xxh128.high64) {
                    throw std::runtime_error(name + " не прошел проверку на целостность (xxh128 high64)");
                }
            }
        }

        std::vector<uint8_t> Block::dump() {
            std::vector<uint8_t> ret = {};

            // [настройки] - settings (начало)
            if(settings.block_number_auto) {
                settings.block_number = data_blocks.size();
            }

            // [название - uint16_t] [название] - name
            uint16_t name_size = name.size();
            std::vector<uint8_t> name_size_vector = IntHelper::uint16_to_uvec8(name_size);
            ret.insert(ret.end(), name_size_vector.begin(), name_size_vector.end());
            ret.insert(ret.end(), name.begin(), name.end());

            // [данные - uint64_t] [данные] - data_blocks / data_main
            std::vector<uint8_t> data;
            for(int i = 0; i < settings.block_number; i++) {
                std::vector<uint8_t> data_blocks_dump = data_blocks[i].dump();
                data.insert(data.end(), data_blocks_dump.begin(), data_blocks_dump.end());
            }
            data.insert(data.end(), data_main.begin(), data_main.end());

            if(settings.compression_type == 1) { // Deflate
                settings.compression_type_1.expected_size = data.size();
                std::vector<uint8_t> compressed(compressBound(data.size()));
                mz_ulong compressed_len = compressed.size();
                int status = mz_compress2(compressed.data(), &compressed_len,
                                       data.data(), data.size(), settings.compression_type_1.level);
                if(status != MZ_OK) {
                    throw std::runtime_error("Deflate compress failed");
                }
                compressed.resize(compressed_len);
                data = std::move(compressed);
            }

            uint64_t data_size = data.size();
            std::vector<uint8_t> data_size_vector = IntHelper::uint64_to_uvec8(data_size);
            ret.insert(ret.end(), data_size_vector.begin(), data_size_vector.end());
            ret.insert(ret.end(), data.begin(), data.end());

            // [настройки] - settings (конец)
            std::vector<uint8_t> settings_vector = settings.dump();
            ret.insert(ret.begin(), settings_vector.begin(), settings_vector.end());

            // [размер блока - uint64_t]
            size_t xxh_size = 0;
            if(settings.xxh3_bit == 0) xxh_size = XXH64_SIZE;
            else if(settings.xxh3_bit == 1) xxh_size = XXH128_SIZE;

            uint64_t block_size = ret.size() + 8 + xxh_size; // 8 - uint64_t
            std::vector<uint8_t> block_size_vector = IntHelper::uint64_to_uvec8(block_size);
            ret.insert(ret.begin(), block_size_vector.begin(), block_size_vector.end());

            // [xxh3 (все предыдущие блоки вместе взятые)] - xxh3 / xxh128
            uint64_t xxh3_64;
            XXH128_hash_t xxh3_128;
            std::vector<uint8_t> xxh3, xxh3_128_buffer;
            switch(settings.xxh3_bit) {
                case 0:
                    xxh3_64 = XXH3_64bits(ret.data(), ret.size());
                    xxh3 = IntHelper::uint64_to_uvec8(xxh3_64);

                    this->xxh3 = xxh3_64;
                    break;
                case 1:
                    xxh3_128 = XXH3_128bits(ret.data(), ret.size());
                    xxh3_128_buffer = IntHelper::uint64_to_uvec8(xxh3_128.low64);
                    xxh3.insert(xxh3.end(), xxh3_128_buffer.begin(), xxh3_128_buffer.end());
                    xxh3_128_buffer = IntHelper::uint64_to_uvec8(xxh3_128.high64);
                    xxh3.insert(xxh3.end(), xxh3_128_buffer.begin(), xxh3_128_buffer.end());

                    this->xxh128 = xxh3_128;
                    break;
                default: break;
            }
            ret.insert(ret.end(), xxh3.begin(), xxh3.end());

            return ret;
        }

#if NONAME24_BLOCKYBINARY_ENABLE_PRINT
        void Block::print(int tab) {
            std::print("{}{}:\n", std::string(tab, ' '), name);
            settings.print(tab);
            tab += 4;
            std::print("{}data_blocks:\n", std::string(tab, ' '));
            for(int i = 0; i < settings.block_number; i++) {
                tab += 4;
                data_blocks[i].print(tab);
                tab -= 4;
            }
            std::print("{}data_main: {}\n", std::string(tab, ' '), std::string(data_main.begin(), data_main.end()));
            switch(settings.xxh3_bit) {
                case 0:
                    std::print("{}xxh3: {}\n", std::string(tab, ' '), IntHelper::uint64_to_hex(xxh3));
                    break;
                case 1:
                    std::print("{}xxh128: {} {}\n", std::string(tab, ' '),
                               IntHelper::uint64_to_hex(xxh128.low64), IntHelper::uint64_to_hex(xxh128.high64));
                    break;
            }
        }
#endif
    };
};
