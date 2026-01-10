/*     ./src/noname24/blockybinary.hpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

namespace NoName24 {
    namespace BlockyBinary {
        // struct BlockBaseSettings
        std::vector<uint8_t> BlockBaseSettings::dump() {
            std::vector<uint8_t> ret = {};

            // block_number
            std::vector<uint8_t> block_number_vector = IntHelper::int32_to_uvec8(block_number);
            ret.insert(ret.end(), block_number_vector.begin(), block_number_vector.end());

            // compression_type
            ret.push_back(compression_type);

            //

            // xxh3_bit
            ret.push_back(xxh3_bit);

            return ret;
        }

        // struct Block
        size_t Block::get_selfsize() {
            size_t selfsize = 0;

            // base_settings
            std::vector<uint8_t> base_settings_vector = base_settings.dump();
            selfsize += base_settings_vector.size();

            // name
            selfsize += name.size();

            // данные
            selfsize += data_main.size();
            for(int i = 0; i < base_settings.block_number; i++) {
                selfsize += data_blocks[i].get_selfsize();
            }

            // xxh 64bit / 128bit
            switch(base_settings.xxh3_bit) {
                case 0: selfsize +=  8; break;
                case 1: selfsize += 16; break;
                default: break;
            }

            return selfsize;
        }

        void Block::parse(std::span<const uint8_t> data) {}
        std::vector<uint8_t> Block::dump() {
            std::vector<uint8_t> ret = {};

            // [обязательные настройки]
            std::vector<uint8_t> base_settings_vector = base_settings.dump();
            ret.insert(ret.end(), base_settings_vector.begin(), base_settings_vector.end());

            // [название - uint16_t] [название]
            uint16_t name_size = name.size();
            std::vector<uint8_t> name_size_vector = IntHelper::int16_to_uvec8(name_size);
            ret.insert(ret.end(), name_size_vector.begin(), name_size_vector.end());
            ret.insert(ret.end(), name.begin(), name.end());

            // [данные - uint64_t] [данные]
            std::vector<uint8_t> data;
            for(int i = 0; i < base_settings.block_number; i++) {
                std::vector<uint8_t> data_blocks_dump = data_blocks[i].dump();
                data.insert(data.end(), data_blocks_dump.begin(), data_blocks_dump.end());
            }
            data.insert(data.end(), data_main.begin(), data_main.end());

            switch(base_settings.compression_type) {
                case 1: // Deflate
                    break;
                default: break;
            }

            uint64_t data_size = data.size();
            std::vector<uint8_t> data_size_vector = IntHelper::int64_to_uvec8(data_size);
            ret.insert(ret.end(), data_size_vector.begin(), data_size_vector.end());
            ret.insert(ret.end(), data.begin(), data.end());

            // [xxh3 (все предыдущие блоки вместе взятые)]
            uint64_t xxh3_64;
            XXH128_hash_t xxh3_128;
            std::vector<uint8_t> xxh3, xxh3_buffer;
            switch(base_settings.xxh3_bit) {
                case 0:
                    xxh3_64 = XXH3_64bits(ret.data(), ret.size());
                    xxh3 = IntHelper::int64_to_uvec8(xxh3_64);
                    break;
                case 1:
                    xxh3_128 = XXH3_128bits(ret.data(), ret.size());
                    xxh3_buffer = IntHelper::int64_to_uvec8(xxh3_128.low64);
                    xxh3.insert(xxh3.end(), xxh3_buffer.begin(), xxh3_buffer.end());
                    xxh3_buffer = IntHelper::int64_to_uvec8(xxh3_128.high64);
                    xxh3.insert(xxh3.end(), xxh3_buffer.begin(), xxh3_buffer.end());
                    break;
                default: break;
            }
            ret.insert(ret.end(), xxh3.begin(), xxh3.end());

            return ret;
        }
    };
};
