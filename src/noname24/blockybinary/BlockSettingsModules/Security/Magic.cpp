/*     ./src/noname24/BlockSettingsModules/Security/Magic.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

#include <ranges> // std::ranges::equal

// Сторонние библиотеки
#include <xxhash.h> // https://github.com/Cyan4973/xxHash

namespace NoName24 {
    namespace BlockyBinary {
        namespace BlockSettingsModules {
            namespace Security {
                Magic::Magic() : BlockSettingsModuleBase("BlockSettingsModules::Security::Magic") {
                    magic_source_to_magic();
                }

                void Magic::magic_source_to_magic() {
                    magic_source_to_magic(magic_source);
                }
                void Magic::magic_source_to_magic(std::span<const uint8_t, 8> magic_source) {
                    uint64_t XXH3_64 = XXH3_64bits(magic_source.data(), magic_source.size());
                    IntHelper::uint64_to_uarray8(XXH3_64, magic);
                }

                size_t Magic::parse_begin_in(std::span<const uint8_t> ret, size_t offset,
                    Debug* debug
                ) {
                    size_t offset_new = 0;

                    if(magic.size() == 0) throw std::runtime_error(name + ": magic пустой");
                    if(ret.size() - (offset + offset_new) < magic_source.size()) throw std::runtime_error(name + ": offset выходит за границы ret");

                    std::span<const uint8_t> magic_buffer = ret.subspan(offset + offset_new, magic_source.size());
                    offset_new += magic_source.size();

                    if(!std::ranges::equal(magic_buffer, magic)) throw std::runtime_error(name + ": magic_buffer не прошел сверку с magic");

                    return offset_new;
                }

                void Magic::dump_begin_in(std::vector<uint8_t>& ret) {
                    if(magic.size() == 0) throw std::runtime_error(name + ": magic пустой");

                    ret.reserve(ret.size() + magic.size());
                    ret.insert(ret.end(), magic.begin(), magic.end());
                }
            };
        };
    };
};
