/*     ./src/noname24/blockybinary/inthelper.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary/inthelper.hpp>

namespace NoName24 {
    namespace BlockyBinary {
        namespace IntHelper {
            // intX_to_vec8
            std::vector<int8_t> int16_to_vec8(int16_t v) {
                return {
                    int8_t(v & 0xFF),
                    int8_t((v >> 8) & 0xFF)
                };
            }
            std::vector<int8_t> int32_to_vec8(int32_t v) {
                return {
                    int8_t(v & 0xFF),
                    int8_t((v >> 8) & 0xFF),
                    int8_t((v >> 16) & 0xFF),
                    int8_t((v >> 24) & 0xFF)
                };
            }
            std::vector<int8_t> int64_to_vec8(int64_t v) {
                return {
                    int8_t(v & 0xFF),
                    int8_t((v >> 8) & 0xFF),
                    int8_t((v >> 16) & 0xFF),
                    int8_t((v >> 24) & 0xFF),
                    int8_t((v >> 32) & 0xFF),
                    int8_t((v >> 40) & 0xFF),
                    int8_t((v >> 48) & 0xFF),
                    int8_t((v >> 56) & 0xFF)
                };
            }

            // intX_to_uvec8
            std::vector<uint8_t> int16_to_uvec8(int16_t v) {
                return {
                    uint8_t(v & 0xFF),
                    uint8_t((v >> 8) & 0xFF)
                };
            }
            std::vector<uint8_t> int32_to_uvec8(int32_t v) {
                return {
                    uint8_t(v & 0xFF),
                    uint8_t((v >> 8) & 0xFF),
                    uint8_t((v >> 16) & 0xFF),
                    uint8_t((v >> 24) & 0xFF)
                };
            }
            std::vector<uint8_t> int64_to_uvec8(int64_t v) {
                return {
                    uint8_t(v & 0xFF),
                    uint8_t((v >> 8) & 0xFF),
                    uint8_t((v >> 16) & 0xFF),
                    uint8_t((v >> 24) & 0xFF),
                    uint8_t((v >> 32) & 0xFF),
                    uint8_t((v >> 40) & 0xFF),
                    uint8_t((v >> 48) & 0xFF),
                    uint8_t((v >> 56) & 0xFF)
                };
            }
        };
    };
};
