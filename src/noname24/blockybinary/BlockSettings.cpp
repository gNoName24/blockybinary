/* ./src/noname24/blockybinary/BlockSettings.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

namespace NoName24 {
    namespace BlockyBinary {
        size_t BlockSettings::parse(std::span<const uint8_t> ret, size_t offset,
                                    Debug* debug
        ) {
            size_t offset_new = 0;

            // block_number
            std::span<const uint8_t> block_number_span = ret.subspan(offset + offset_new, 4);
            block_number = IntHelper::uspan8_to_uint32(block_number_span);
            offset_new += 4;

            // modules
            for(int i = 0; i < modules.size(); i++) {
                modules[i]->enable = ret[offset + offset_new];
                offset_new += 1;

                if(!modules[i]->enable) continue;
                offset_new += modules[i]->parse(ret, offset + offset_new);
            }

            return offset_new;
        }

        std::vector<uint8_t> BlockSettings::dump_ret() {
            std::vector<uint8_t> ret;
            dump_to(ret);
            return ret;
        }
        void BlockSettings::dump_to(std::vector<uint8_t>& ret) {
            // block_number
            std::array<uint8_t, 4> block_number_vector = IntHelper::uint32_to_uarray8(block_number);
            ret.insert(ret.end(), block_number_vector.begin(), block_number_vector.end());

            // modules
            for(int i = 0; i < modules.size(); i++) {
                ret.push_back(modules[i]->enable);

                if(!modules[i]->enable) continue;
                modules[i]->dump_to(ret);
            }
        }
    };
};
