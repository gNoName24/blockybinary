/*     ./src/noname24/BlockSettingsModules/Security/XXH3.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

// Сторонние библиотеки
#include <xxhash.h> // https://github.com/Cyan4973/xxHash

namespace NoName24 {
    namespace BlockyBinary {
        namespace BlockSettingsModules {
            namespace Security {
                XXH3::XXH3() : BlockSettingsModuleBase("BlockSettingsModules::Security::XXH3") {}

                size_t XXH3::parse_end_out(std::span<const uint8_t> ret, size_t offset,
                    Debug* debug
                ) {
                    size_t xxh3_size = (xxh3_bit == 0) ? 8 : 16;

                    const uint8_t* data_ptr = ret.data();
                    size_t data_len = offset;

                    if(offset + xxh3_size > ret.size()) {
                        throw std::runtime_error(name + ": недостаточно данных");
                    }

                    std::span<const uint8_t> xxh3_span = ret.subspan(offset, xxh3_size);

                    if(xxh3_bit == 0) {
                        uint64_t target = IntHelper::uspan8_to_uint64(xxh3_span);
                        uint64_t current = XXH3_64bits(data_ptr, data_len);
                        if(debug) {
                            debug->print("target:  ", IntHelper::uint64_to_hex(target));
                            debug->print("current: ", IntHelper::uint64_to_hex(current));
                        }
                        if(current != target) throw std::runtime_error(name + ": неудачное сравнение (XXH3_64)");
                    } else {
                        XXH128_hash_t target;
                        target.low64 = IntHelper::uspan8_to_uint64(xxh3_span.subspan(0, 8));
                        target.high64 = IntHelper::uspan8_to_uint64(xxh3_span.subspan(8, 8));
                        XXH128_hash_t current = XXH3_128bits(data_ptr, data_len);
                        if(debug) {
                            debug->print("target:  ", IntHelper::uint64_to_hex(target.low64), " ", IntHelper::uint64_to_hex(target.high64));
                            debug->print("current: ", IntHelper::uint64_to_hex(current.low64), " ", IntHelper::uint64_to_hex(current.high64));
                        }
                        if(current.low64 != target.low64 || current.high64 != target.high64)
                            throw std::runtime_error(name + ": неудачное сравнение (XXH3_128)");
                    }

                    return xxh3_size;
                }
                void XXH3::dump_end_out(
                    std::vector<uint8_t>& ret
                ) {
                    uint64_t xxh3_64;
                    XXH128_hash_t xxh3_128;

                    if(xxh3_bit == 0) {
                        xxh3_64 = XXH3_64bits(ret.data(), ret.size());
                        std::array<uint8_t, 8> buf = IntHelper::uint64_to_uarray8(xxh3_64);
                        ret.insert(ret.end(), buf.begin(), buf.end());
                    } else {
                        xxh3_128 = XXH3_128bits(ret.data(), ret.size());
                        std::array<uint8_t, 8> l = IntHelper::uint64_to_uarray8(xxh3_128.low64);
                        std::array<uint8_t, 8> h = IntHelper::uint64_to_uarray8(xxh3_128.high64);
                        ret.insert(ret.end(), l.begin(), l.end());
                        ret.insert(ret.end(), h.begin(), h.end());
                    }
                }

                size_t XXH3::parse(std::span<const uint8_t> ret, size_t offset,
                    Debug* debug
                ) {
                    size_t offset_new = 0;

                    xxh3_bit = ret[offset + offset_new];
                    offset_new += 1;

                    return offset_new;
                }
                void XXH3::dump_to(std::vector<uint8_t>& ret) {
                    // xxh3_bit
                    ret.push_back(xxh3_bit);
                }
            };
        };
    };
};
