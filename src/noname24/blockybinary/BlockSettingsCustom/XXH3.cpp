/*     ./src/noname24/BlockSettingsCustom/XXH3.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

namespace NoName24 {
    namespace BlockyBinary {
        void BlockSettingsModule_XXH3::dump_end_out(
            std::vector<uint8_t>& ret
        ) {
            uint64_t xxh3_64;
            XXH128_hash_t xxh3_128;

            switch(xxh3_bit) {
                case 0: xxh3_64 = XXH3_64bits(ret.data(), ret.size());
                case 1: xxh3_128 = XXH3_128bits(ret.data(), ret.size());
            }
            std::vector<uint8_t> xxh3;
            if(xxh3_bit == 0) {
                std::array<uint8_t, 8> xxh3_buffer = IntHelper::uint64_to_uarray8(xxh3_64);
                xxh3.insert(xxh3.end(), xxh3_buffer.begin(), xxh3_buffer.end());
            } else
            if(xxh3_bit == 1) {
                std::array<uint8_t, 8> xxh3_128_buffer;
                xxh3_128_buffer = IntHelper::uint64_to_uarray8(xxh3_128.low64);
                xxh3.insert(xxh3.end(), xxh3_128_buffer.begin(), xxh3_128_buffer.end());
                xxh3_128_buffer = IntHelper::uint64_to_uarray8(xxh3_128.high64);
                xxh3.insert(xxh3.end(), xxh3_128_buffer.begin(), xxh3_128_buffer.end());
            }
            ret.insert(ret.end(), xxh3.begin(), xxh3.end());
        }

        void BlockSettingsModule_XXH3::dump_to(std::vector<uint8_t>& ret) {
            // xxh3_bit
            ret.push_back(xxh3_bit);
        }
    };
};
