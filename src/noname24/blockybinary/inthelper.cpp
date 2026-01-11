/*     ./src/noname24/blockybinary/inthelper.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary/inthelper.hpp>

#include <stdexcept> // std::runtime_error

namespace NoName24 {
    namespace BlockyBinary {
        namespace IntHelper {
            // uintX_to_uvec8
            std::vector<uint8_t> uint16_to_uvec8(uint16_t v) {
                return {
                    uint8_t(v & 0xFF),
                    uint8_t((v >> 8) & 0xFF)
                };
            }
            std::vector<uint8_t> uint32_to_uvec8(uint32_t v) {
                return {
                    uint8_t(v & 0xFF),
                    uint8_t((v >> 8) & 0xFF),
                    uint8_t((v >> 16) & 0xFF),
                    uint8_t((v >> 24) & 0xFF)
                };
            }
            std::vector<uint8_t> uint64_to_uvec8(uint64_t v) {
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

            // uvec8_to_uintX
            uint16_t uvec8_to_uint16(const std::vector<uint8_t>& v) {
                if(v.size() < 2)
                    throw std::runtime_error("uvec8_to_uint16: vector too small");
                return static_cast<uint16_t>(
                    (uint16_t(uint8_t(v[0]))) |
                    (uint16_t(uint8_t(v[1])) << 8)
                );
            }
            uint32_t uvec8_to_uint32(const std::vector<uint8_t>& v) {
                if(v.size() < 4)
                    throw std::runtime_error("uvec8_to_uint32: vector too small");
                return static_cast<uint32_t>(
                    (uint32_t(uint8_t(v[0]))) |
                    (uint32_t(uint8_t(v[1])) << 8) |
                    (uint32_t(uint8_t(v[2])) << 16) |
                    (uint32_t(uint8_t(v[3])) << 24)
                );
            }
            uint64_t uvec8_to_uint64(const std::vector<uint8_t>& v) {
                if(v.size() < 8)
                    throw std::runtime_error("uvec8_to_uint64: vector too small");
                return static_cast<uint64_t>(
                    (uint64_t(uint8_t(v[0]))) |
                    (uint64_t(uint8_t(v[1])) << 8) |
                    (uint64_t(uint8_t(v[2])) << 16) |
                    (uint64_t(uint8_t(v[3])) << 24) |
                    (uint64_t(uint8_t(v[4])) << 32) |
                    (uint64_t(uint8_t(v[5])) << 40) |
                    (uint64_t(uint8_t(v[6])) << 48) |
                    (uint64_t(uint8_t(v[7])) << 56)
                );
            }
        };
    };
};
