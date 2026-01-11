/* ./include/noname24/blockybinary/inthelper.hpp
 * NoName24 - 2026
 */
#ifndef NONAME24_BLOCKYBINARY_INTHELPER_HPP
#define NONAME24_BLOCKYBINARY_INTHELPER_HPP

#include <cstdint> // u/intX_t
#include <vector> // std::vector

namespace NoName24 {
    namespace BlockyBinary {
        namespace IntHelper {
            // uintX_to_uvec8
            std::vector<uint8_t> uint16_to_uvec8(uint16_t v);
            std::vector<uint8_t> uint32_to_uvec8(uint32_t v);
            std::vector<uint8_t> uint64_to_uvec8(uint64_t v);

            // uvec8_to_uintX
            uint16_t uvec8_to_uint16(const std::vector<uint8_t>& v);
            uint32_t uvec8_to_uint32(const std::vector<uint8_t>& v);
            uint64_t uvec8_to_uint64(const std::vector<uint8_t>& v);
        };
    };
};

#endif // NONAME24_BLOCKYBINARY_INTHELPER_HPP
