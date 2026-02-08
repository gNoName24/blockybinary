/* ./src/noname24/BlockSettingsModules/CompressDecompress/DeflateInflate.cpp
 * noname24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

// Сторонние библиотеки
#include <miniz/miniz.h> // https://github.com/richgel999/miniz

namespace noname24 {
    namespace BlockyBinary {
        namespace BlockSettingsModules {
            namespace CompressDecompress {
                DeflateInflate::DeflateInflate() : BlockSettingsModuleBase("BlockSettingsModules::CompressDecompress::DeflateInflate") {
                    level = MZ_DEFAULT_LEVEL;
                    strategy = MZ_DEFAULT_STRATEGY;
                }

                size_t DeflateInflate::parse_data_in(std::span<const uint8_t> ret, size_t offset,
                                                     std::array<uint8_t, 8>& data_size_array,
                                                     std::vector<uint8_t>& data_vector,
                                                     Debug* debug
                ) {
                    if(data_vector.empty()) return {};
                    std::vector<uint8_t> data_uncompress(expected_size);

                    mz_stream stream{};
                    int mz_inflateInit2_status = mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS);
                    if(mz_inflateInit2_status != MZ_OK) {
                        throw std::runtime_error(name + ": inflate failed (mz_inflateInit2_status != MZ_OK)");
                    }

                    stream.next_in = data_vector.data();
                    stream.avail_in = static_cast<unsigned int>(data_vector.size());

                    stream.next_out = data_uncompress.data();
                    stream.avail_out = static_cast<unsigned int>(data_uncompress.size());

                    int mz_inflate_status = mz_inflate(&stream, MZ_FINISH);
                    mz_inflateEnd(&stream);

                    if(mz_inflate_status != MZ_STREAM_END) throw std::runtime_error(name + ": inflate failed (mz_inflate_status != MZ_STREAM_END)");

                    data_uncompress.resize(static_cast<size_t>(stream.total_out));

                    IntHelper::uint64_to_uarray8(expected_size, data_size_array);
                    data_vector = std::move(data_uncompress);

                    return 0;
                }
                void DeflateInflate::dump_data_in(std::vector<uint8_t>& ret,
                                                  uint64_t& data_size,
                                                  std::array<uint8_t, 8>& data_size_array,
                                                  std::vector<Block>& data_blocks,
                                                  std::vector<uint8_t>& data_main,
                                                  std::vector<uint8_t>& data
                ) {
                    std::vector<uint8_t> buffer;

                    expected_size = data.size();

                    mz_stream stream{};

                    int mz_deflateInit2_status = mz_deflateInit2(
                        &stream,
                        level,
                        MZ_DEFLATED,
                        -MZ_DEFAULT_WINDOW_BITS,
                        9,
                        strategy
                    );
                    if(mz_deflateInit2_status != MZ_OK) {
                        throw std::runtime_error(name + ": deflate failed (mz_deflateInit2_status != MZ_OK)");
                    }

                    stream.next_in = data.data();
                    stream.avail_in = static_cast<unsigned int>(data.size());

                    uint8_t out[1024 * 16];

                    while(true) {
                        stream.next_out  = out;
                        stream.avail_out = sizeof(out);

                        int mz_deflate_status = mz_deflate(&stream, MZ_FINISH);

                        size_t produced = sizeof(out) - stream.avail_out;
                        buffer.insert(buffer.end(), out, out + produced);

                        if(mz_deflate_status == MZ_STREAM_END) {
                            break;
                        }
                        if(mz_deflate_status != MZ_OK) {
                            mz_deflateEnd(&stream);
                            throw std::runtime_error(name + ": deflate failed (mz_deflate_status != MZ_OK)");
                        }
                    }

                    mz_deflateEnd(&stream);

                    data = buffer;
                    data_size = data.size();
                    data_size_array = IntHelper::uint64_to_uarray8(data_size);
                }

                size_t DeflateInflate::parse(std::span<const uint8_t> ret, size_t offset,
                                             Debug* debug
                ) {
                    size_t offset_new = 0;

                    // level
                    level = ret[offset + offset_new];
                    offset_new += 1;

                    // strategy
                    strategy = ret[offset + offset_new];
                    offset_new += 1;

                    // expected_size
                    std::span<const uint8_t> expected_size_span = ret.subspan(offset + offset_new, 8);
                    expected_size = IntHelper::uspan8_to_uint64(expected_size_span);
                    offset_new += 8;

                    return offset_new;
                }
                void DeflateInflate::dump_to(std::vector<uint8_t>& ret) {
                    // level
                    ret.push_back(level);

                    // strategy
                    ret.push_back(strategy);

                    // expected_size
                    std::array<uint8_t, 8> expected_size_array = IntHelper::uint64_to_uarray8(expected_size);
                    ret.insert(ret.end(), expected_size_array.begin(), expected_size_array.end());
                }
            };
        };
    };
};
