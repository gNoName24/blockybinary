/* ./include/noname24/blockybinary/inthelper.hpp
 * noname24 - 2026
 */
#pragma once
#ifndef NONAME24_BLOCKYBINARY_INTHELPER_HPP
#define NONAME24_BLOCKYBINARY_INTHELPER_HPP

#include <cstdint> // u/intN_t
#include <span> // std::span
#include <array> // std::array
#include <string> // std::string

namespace noname24 {
    namespace BlockyBinary {
        namespace IntHelper {
            // uintN_to_hex
            std::string uint8_to_hex(const uint8_t& v);
            std::string uint16_to_hex(const uint16_t& v);
            std::string uint32_to_hex(const uint32_t& v);
            std::string uint64_to_hex(const uint64_t& v);

            // uintN_to_uarray8
            std::array<uint8_t, 2> uint16_to_uarray8(const uint16_t& value);
            void uint16_to_uarray8(const uint16_t& value, std::span<uint8_t, 2> out);
            std::array<uint8_t, 4> uint32_to_uarray8(const uint32_t& value);
            void uint32_to_uarray8(const uint32_t& value, std::span<uint8_t, 4> out);
            std::array<uint8_t, 8> uint64_to_uarray8(const uint64_t& value);
            void uint64_to_uarray8(const uint64_t& value, std::span<uint8_t, 8> out);

            // uspan8_to_uintN
            uint16_t uspan8_to_uint16(std::span<const uint8_t> v);
            uint32_t uspan8_to_uint32(std::span<const uint8_t> v);
            uint64_t uspan8_to_uint64(std::span<const uint8_t> v);
        };
    };
};

#endif // NONAME24_BLOCKYBINARY_INTHELPER_HPP
