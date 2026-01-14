/*     ./src/noname24/blockybinary/Block.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

#include <algorithm> // std::copy
#if NONAME24_BLOCKYBINARY_ENABLE_PRINT
    #include <print> // std::print
#endif

namespace NoName24 {
    namespace BlockyBinary {
        // magic
        void Block::magic_generate() {
            magic_generate(magic_source);
        }
        void Block::magic_generate(std::span<const uint8_t> magic_source) {
            magic = XXH3_64bits(magic_source.data(), magic_source.size());
        }

        size_t Block::get_selfsize() {
            size_t selfsize = 0;

            std::vector<uint8_t> dump_vector;
            dump_to(dump_vector);
            selfsize += dump_vector.size();

            return selfsize;
        }

        void Block::add_block(const Block& block) {
            data_blocks_indexed[block.name] = data_blocks.size();
            data_blocks.push_back(block);
        }
        void Block::add_block(Block&& block) {
            data_blocks_indexed[block.name] = data_blocks.size();
            data_blocks.push_back(std::move(block));
        }

        size_t Block::parse(std::span<const uint8_t> data) {
            size_t byte_shift = 0; // сдвиг

            // размер блока
            std::span<const uint8_t> block_size_span = data.subspan(byte_shift, 8);
            uint64_t block_size = IntHelper::uspan8_to_uint64(block_size_span);
            byte_shift += 8;

            // base_settings
            BlockSettings settings;
            byte_shift += settings.parse(data.subspan(byte_shift));

            this->settings = settings;

            // name
            std::span<const uint8_t> name_size_span = data.subspan(byte_shift, 2);
            uint16_t name_size = IntHelper::uspan8_to_uint16(name_size_span);
            byte_shift += 2;
            name.assign(reinterpret_cast<const char*>(data.data() + byte_shift), name_size);
            byte_shift += name_size;

            // данные
            std::span<const uint8_t> data_size_compressed_span = data.subspan(byte_shift, 8);
            uint64_t data_size_compressed = IntHelper::uspan8_to_uint64(data_size_compressed_span);
            byte_shift += 8;

            std::span<const uint8_t> data_compressed = data.subspan(byte_shift, data_size_compressed);
            std::vector<uint8_t> data_uncompressed_storage;
            std::span<const uint8_t> data_uncompressed;
            if(settings.compression_type == 1) {
                data_uncompressed_storage = settings.compression_type_1.uncompress(data_compressed);
                data_uncompressed = data_uncompressed_storage;
            } else {
                data_uncompressed = data_compressed;
            }

            uint64_t data_size_uncompressed = data_uncompressed.size();

            size_t byte_shift_2 = 0;
            for(uint64_t i = 0; i < settings.block_number; i++) {
                std::span<const uint8_t> data_blocks_block_size_span = data_uncompressed.subspan(byte_shift_2, 8);
                uint64_t data_blocks_block_size = IntHelper::uspan8_to_uint64(data_blocks_block_size_span);

                std::span<const uint8_t> data_blocks_block_data = data_uncompressed.subspan(byte_shift_2, data_blocks_block_size);
                Block data_blocks_block;
                data_blocks_block.parse(data_blocks_block_data);

                add_block(data_blocks_block);

                byte_shift_2 += data_blocks_block_size;
                data_size_uncompressed -= data_blocks_block_size;
            }

            std::span<const uint8_t> data_main_span = data_uncompressed.subspan(byte_shift_2, data_size_uncompressed);
            data_main = std::vector<uint8_t>(data_main_span.begin(), data_main_span.end());

            byte_shift += data_size_compressed;

            // xxh3 / xxh128
            if(settings.xxh3_bit == 0) {
                std::span<const uint8_t> xxh3_span = data.subspan(byte_shift, XXH64_SIZE);
                xxh3 = IntHelper::uspan8_to_uint64(xxh3_span);
                byte_shift += XXH64_SIZE;
            } else
            if(settings.xxh3_bit == 1) {
                std::span<const uint8_t> xxh128_span = data.subspan(byte_shift, XXH128_SIZE);
                xxh128.low64 = IntHelper::uspan8_to_uint64(
                    std::span<const uint8_t>(xxh128_span.begin(), xxh128_span.begin() + XXH128_SIZE / 2)
                );
                xxh128.high64 = IntHelper::uspan8_to_uint64(
                    std::span<const uint8_t>(xxh128_span.begin() + XXH128_SIZE / 2, xxh128_span.begin() + XXH128_SIZE)
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

            return byte_shift;
        }

        std::vector<uint8_t> Block::dump_ret() {
            std::vector<uint8_t> ret;
            dump_to(ret);
            return ret;
        }
        void Block::dump_to(std::vector<uint8_t>& ret) {
            // NOTE: ПЕРЕПИСЬ

            settings.block_number = data_blocks.size();

            // magic
            if(magic == 0x000000000000000000) {
                throw std::runtime_error("magic не сгенерирован");
            }
            std::array<uint8_t, 8> magic_array = IntHelper::uint64_to_uarray8(magic);
            ret.insert(ret.end(), magic_array.begin(), magic_array.end());

            // settings - in
            std::vector<uint8_t> settings_vector;

            // name - in
            uint16_t name_size = name.size();
            std::array<uint8_t, 2> name_size_array = IntHelper::uint16_to_uarray8(name_size);

            // data - in
            std::vector<uint8_t> data;
            for(uint64_t i = 0; i < settings.block_number; i++) {
                data_blocks[i].dump_to(data);
            }
            data.insert(data.end(), data_main.begin(), data_main.end());
            if(settings.compression_type == 1) { // Deflate
                data = settings.compression_type_1.compress(data);
            }
            uint64_t data_size = data.size();
            std::array<uint8_t, 8> data_size_array = IntHelper::uint64_to_uarray8(data_size);

            // xxh3 - in
            std::vector<uint8_t> xxh3;
            if(settings.xxh3_bit == 0) {
                uint64_t xxh3_64 = XXH3_64bits(ret.data(), ret.size());
                std::array<uint8_t, 8> xxh3_buffer = IntHelper::uint64_to_uarray8(xxh3_64);
                xxh3.insert(xxh3.end(), xxh3_buffer.begin(), xxh3_buffer.end());

                this->xxh3 = xxh3_64;
            } else
            if(settings.xxh3_bit == 1) {
                std::array<uint8_t, 8> xxh3_128_buffer;
                XXH128_hash_t xxh3_128 = XXH3_128bits(ret.data(), ret.size());
                xxh3_128_buffer = IntHelper::uint64_to_uarray8(xxh3_128.low64);
                xxh3.insert(xxh3.end(), xxh3_128_buffer.begin(), xxh3_128_buffer.end());
                xxh3_128_buffer = IntHelper::uint64_to_uarray8(xxh3_128.high64);
                xxh3.insert(xxh3.end(), xxh3_128_buffer.begin(), xxh3_128_buffer.end());

                this->xxh128 = xxh3_128;
            }

            // settings - out
            settings.dump_to(settings_vector);
            ret.insert(ret.end(), settings_vector.begin(), settings_vector.end());

            // name - out
            ret.insert(ret.end(), name_size_array.begin(), name_size_array.end());
            ret.insert(ret.end(), name.begin(), name.end());

            // data - out
            ret.insert(ret.end(), data_size_array.begin(), data_size_array.end());
            ret.insert(ret.end(), data.begin(), data.end());

            // xxh3 - out
            ret.insert(ret.end(), xxh3.begin(), xxh3.end());

            /*
            // [настройки] - settings (начало)
            if(settings.block_number_auto) {
                settings.block_number = data_blocks.size();
            }

            // [название - uint16_t] [название] - name
            uint16_t name_size = name.size();
            std::array<uint8_t, 2> name_size_array = IntHelper::uint16_to_uarray8(name_size);
            ret.insert(ret.end(), name_size_array.begin(), name_size_array.end());
            ret.insert(ret.end(), name.begin(), name.end());

            // [данные - uint64_t] [данные] - data_blocks / data_main
            std::vector<uint8_t> data;
            for(uint64_t i = 0; i < settings.block_number; i++) {
                data_blocks[i].dump_to(data);
            }
            data.insert(data.end(), data_main.begin(), data_main.end());

            if(settings.compression_type == 1) { // Deflate
                settings.compression_type_1.expected_size = data.size();
                data = settings.compression_type_1.compress(data);
            }

            uint64_t data_size = data.size();
            std::array<uint8_t, 8> data_size_array = IntHelper::uint64_to_uarray8(data_size);
            ret.insert(ret.end(), data_size_array.begin(), data_size_array.end());
            ret.insert(ret.end(), data.begin(), data.end());

            // [настройки] - settings (конец)
            std::vector<uint8_t> settings_vector;
            settings.dump_to(settings_vector);
            ret.insert(ret.begin(), settings_vector.begin(), settings_vector.end()); // NOTE: ИЗ-ЗА ret.begin() ВСЁ ЛОМАЕТСЯ, ИСПРАВЬ

            // [размер блока - uint64_t]
            size_t xxh_size = 0;
            if(settings.xxh3_bit == 0) xxh_size = XXH64_SIZE;
            else if(settings.xxh3_bit == 1) xxh_size = XXH128_SIZE;

            uint64_t block_size = ret.size() + 8 + xxh_size; // 8 - uint64_t
            std::array<uint8_t, 8> block_size_array = IntHelper::uint64_to_uarray8(block_size);
            ret.insert(ret.begin(), block_size_array.begin(), block_size_array.end());

            // [xxh3 (все предыдущие блоки вместе взятые)] - xxh3 / xxh128
            if(settings.xxh3_bit == 0) {
                std::array<uint8_t, 8> xxh3;
                uint64_t xxh3_64 = XXH3_64bits(ret.data(), ret.size());
                xxh3 = IntHelper::uint64_to_uarray8(xxh3_64);

                this->xxh3 = xxh3_64;
                ret.insert(ret.end(), xxh3.begin(), xxh3.end());
            } else
            if(settings.xxh3_bit == 1) {
                std::array<uint8_t, 16> xxh128;
                std::array<uint8_t, 8> xxh3_128_buffer;
                XXH128_hash_t xxh3_128 = XXH3_128bits(ret.data(), ret.size());
                xxh3_128_buffer = IntHelper::uint64_to_uarray8(xxh3_128.low64);
                ret.insert(ret.end(), xxh3_128_buffer.begin(), xxh3_128_buffer.end());
                xxh3_128_buffer = IntHelper::uint64_to_uarray8(xxh3_128.high64);
                ret.insert(ret.end(), xxh3_128_buffer.begin(), xxh3_128_buffer.end());

                this->xxh128 = xxh3_128;
            }
            */
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
