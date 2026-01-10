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
            // intX_to_vec8
            std::vector<int8_t> int16_to_vec8(int16_t v);
            std::vector<int8_t> int32_to_vec8(int32_t v);
            std::vector<int8_t> int64_to_vec8(int64_t v);

            // intX_to_uvec8
            std::vector<uint8_t> int16_to_uvec8(int16_t v);
            std::vector<uint8_t> int32_to_uvec8(int32_t v);
            std::vector<uint8_t> int64_to_uvec8(int64_t v);
        };
    };
};

#endif // NONAME24_BLOCKYBINARY_INTHELPER_HPP
