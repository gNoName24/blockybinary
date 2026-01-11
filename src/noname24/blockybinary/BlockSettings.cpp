/*     ./src/noname24/BlockSettings.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

#include <print> // std::print

namespace NoName24 {
    namespace BlockyBinary {
        size_t BlockSettings::get_selfsize() {
            size_t selfsize = 0;

            std::vector<uint8_t> dump_vector = dump();
            selfsize += dump_vector.size();

            return selfsize;
        }

        uint64_t BlockSettings::parse(std::span<const uint8_t> data) {
            uint64_t byte_shift = 0; // сдвиг

            std::span<const uint8_t> block_number_span = data.subspan(byte_shift, 4);
            block_number = IntHelper::uvec8_to_uint32(
                std::vector<uint8_t>(block_number_span.begin(), block_number_span.end())
            );
            byte_shift += 4;

            compression_type = data[byte_shift];
            byte_shift += 1;

            if(compression_type == 1) {
                std::span<const uint8_t> compression_type_1_span = data.subspan(byte_shift, BlockSettings_Deflate::MAX_SIZE_BYTE);
                byte_shift += compression_type_1.parse(compression_type_1_span);
            }

            xxh3_bit = data[byte_shift];
            byte_shift += 1;

            return byte_shift;
        }

        std::vector<uint8_t> BlockSettings::dump() {
            std::vector<uint8_t> ret = {};

            // 1 - block_number
            std::vector<uint8_t> block_number_vector = IntHelper::uint32_to_uvec8(block_number);
            ret.insert(ret.end(), block_number_vector.begin(), block_number_vector.end());

            // 2 - compression_type
            ret.push_back(compression_type);

            // ?3 - compression_type_1
            if(compression_type == 1) {
                std::vector<uint8_t> compression_type_1_dump = compression_type_1.dump();
                ret.insert(ret.end(), compression_type_1_dump.begin(), compression_type_1_dump.end());
            }

            // 4 - xxh3_bit
            ret.push_back(xxh3_bit);

            return ret;
        }

        void BlockSettings::print(int tab) {
            tab += 4;
            std::print("{}base_settings:\n", std::string(tab, ' '));
            std::print("{}    block_number: {}\n", std::string(tab, ' '), block_number);
            std::print("{}    compression_type: {}\n", std::string(tab, ' '), compression_type);
            if(compression_type == 1) {
                std::print("{}    compression_type_1:\n", std::string(tab, ' '));
                std::print("{}        level: {}\n", std::string(tab, ' '), compression_type_1.level);
                std::print("{}        expected_size: {}\n", std::string(tab, ' '), compression_type_1.expected_size);
            }
            std::print("{}    xxh3_bit: {}\n", std::string(tab, ' '), xxh3_bit);
        }
    };
};
