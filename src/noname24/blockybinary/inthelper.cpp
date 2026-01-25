/* ./src/noname24/blockybinary/inthelper.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary/inthelper.hpp>

#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream
#include <iomanip> // std::setfill ; std::setw

namespace noname24 {
    namespace BlockyBinary {
        namespace IntHelper {
            // uintN_to_hex
            std::string uint8_to_hex(const uint8_t& v) {
                std::stringstream ss;
                ss << std::hex << std::uppercase << std::setfill('0');
                for(int i = 0; i < 1; ++i) { ss << std::setw(2) << ((v >> (i * 8)) & 0xFF); }
                return ss.str();
            }
            std::string uint16_to_hex(const uint16_t& v) {
                std::stringstream ss;
                ss << std::hex << std::uppercase << std::setfill('0');
                for(int i = 0; i < 2; ++i) { ss << std::setw(2) << ((v >> (i * 8)) & 0xFF); }
                return ss.str();
            }
            std::string uint32_to_hex(const uint32_t& v) {
                std::stringstream ss;
                ss << std::hex << std::uppercase << std::setfill('0');
                for(int i = 0; i < 4; ++i) { ss << std::setw(2) << ((v >> (i * 8)) & 0xFF); }
                return ss.str();
            }
            std::string uint64_to_hex(const uint64_t& v) {
                std::stringstream ss;
                ss << std::hex << std::uppercase << std::setfill('0');
                for(int i = 0; i < 8; ++i) { ss << std::setw(2) << ((v >> (i * 8)) & 0xFF); }
                return ss.str();
            }

            // uintN_to_uarray8
            std::array<uint8_t, 2> uint16_to_uarray8(const uint16_t& v) {
                return {
                    uint8_t(v & 0xFF),
                    uint8_t((v >> 8) & 0xFF)
                };
            }
            void uint16_to_uarray8(const uint16_t& value, std::span<uint8_t, 2> out) {
                out[0] = value & 0xFF;
                out[1] = (value >> 8) & 0xFF;
            }
            std::array<uint8_t, 4> uint32_to_uarray8(const uint32_t& v) {
                return {
                    uint8_t(v & 0xFF),
                    uint8_t((v >> 8) & 0xFF),
                    uint8_t((v >> 16) & 0xFF),
                    uint8_t((v >> 24) & 0xFF)
                };
            }
            void uint32_to_uarray8(const uint32_t& value, std::span<uint8_t, 4> out) {
                out[0] = value & 0xFF;
                out[1] = (value >> 8) & 0xFF;
                out[2] = (value >> 16) & 0xFF;
                out[3] = (value >> 24) & 0xFF;
            }
            std::array<uint8_t, 8> uint64_to_uarray8(const uint64_t& v) {
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
            void uint64_to_uarray8(const uint64_t& value, std::span<uint8_t, 8> out) {
                out[0] = value & 0xFF;
                out[1] = (value >> 8) & 0xFF;
                out[2] = (value >> 16) & 0xFF;
                out[3] = (value >> 24) & 0xFF;
                out[4] = (value >> 32) & 0xFF;
                out[5] = (value >> 40) & 0xFF;
                out[6] = (value >> 48) & 0xFF;
                out[7] = (value >> 56) & 0xFF;
            }

            // uspan8_to_uintN
            uint16_t uspan8_to_uint16(std::span<const uint8_t> v) {
                if(v.size() < 2) throw std::runtime_error("uspan8_to_uint16: span too small");
                return static_cast<uint16_t>(
                    (uint16_t(uint8_t(v[0]))) |
                    (uint16_t(uint8_t(v[1])) << 8)
                );
            }
            uint32_t uspan8_to_uint32(std::span<const uint8_t> v) {
                if(v.size() < 4) throw std::runtime_error("uspan8_to_uint32: span too small");
                return static_cast<uint32_t>(
                    (uint32_t(uint8_t(v[0]))) |
                    (uint32_t(uint8_t(v[1])) << 8) |
                    (uint32_t(uint8_t(v[2])) << 16) |
                    (uint32_t(uint8_t(v[3])) << 24)
                );
            }
            uint64_t uspan8_to_uint64(std::span<const uint8_t> v) {
                if(v.size() < 8) throw std::runtime_error("uspan8_to_uint64: span too small");
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
