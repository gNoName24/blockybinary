/*     ./src/noname24/BlockBaseSettings/Deflate.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

namespace NoName24 {
    namespace BlockyBinary {
        /*std::vector<uint8_t> BlockSettings_Deflate::compress(std::span<const uint8_t> uncompressed) {
            std::vector<uint8_t> ret;

            expected_size = uncompressed.size();

            mz_stream stream{};

            int mz_deflateInit2_status = mz_deflateInit2(
                &stream,
                level,
                MZ_DEFLATED,
                -MZ_DEFAULT_WINDOW_BITS,
                9,
                MZ_DEFAULT_STRATEGY
            );
            if(mz_deflateInit2_status != MZ_OK) {
                throw std::runtime_error("deflate failed (mz_deflateInit2_status != MZ_OK)");
            }

            stream.next_in = uncompressed.data();
            stream.avail_in = static_cast<unsigned int>(uncompressed.size());

            uint8_t out[1024 * 32];

            while(true) {
                stream.next_out  = out;
                stream.avail_out = sizeof(out);

                int mz_deflate_status = mz_deflate(&stream, MZ_FINISH);

                size_t produced = sizeof(out) - stream.avail_out;
                ret.insert(ret.end(), out, out + produced);

                if(mz_deflate_status == MZ_STREAM_END) {
                    break;
                }
                if(mz_deflate_status != MZ_OK) {
                    mz_deflateEnd(&stream);
                    throw std::runtime_error("deflate failed (mz_deflate_status != MZ_OK)");
                }
            }

            mz_deflateEnd(&stream);

            return ret;
        }
        std::vector<uint8_t> BlockSettings_Deflate::uncompress(std::span<const uint8_t> compressed) {
            if(compressed.empty() || expected_size == 0) return {};
            std::vector<uint8_t> ret(expected_size);

            mz_stream stream{};
            int mz_inflateInit2_status = mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS);
            if(mz_inflateInit2_status != MZ_OK) {
                throw std::runtime_error("inflate failed (mz_inflateInit2_status != MZ_OK)");
            }

            stream.next_in = compressed.data();
            stream.avail_in = static_cast<unsigned int>(compressed.size());

            stream.next_out = ret.data();
            stream.avail_out = static_cast<unsigned int>(ret.size());

            int mz_inflate_status = mz_inflate(&stream, MZ_FINISH);
            mz_inflateEnd(&stream);

            if(mz_inflate_status != MZ_STREAM_END) {
                throw std::runtime_error("inflate failed (mz_inflate_status != MZ_STREAM_END)");
            }

            ret.resize(static_cast<size_t>(stream.total_out));

            return ret;
        }

        size_t BlockSettings_Deflate::parse(std::span<const uint8_t> data) {
            size_t byte_shift = 0; // сдвиг

            // level
            level = data[byte_shift];
            byte_shift += 1;

            // expected_size
            std::span<const uint8_t> expected_size_span = data.subspan(byte_shift, 8);
            expected_size = IntHelper::uspan8_to_uint32(expected_size_span);
            byte_shift += 8;

            return byte_shift;
        }
        std::vector<uint8_t> BlockSettings_Deflate::dump_ret() {
            std::vector<uint8_t> ret;
            dump_to(ret);
            return ret;
        }
        void BlockSettings_Deflate::dump_to(std::vector<uint8_t>& ret) {
            // level
            ret.push_back(level);

            // expected_size
            std::array<uint8_t, 8> expected_size_vector = IntHelper::uint64_to_uarray8(expected_size);
            ret.insert(ret.end(), expected_size_vector.begin(), expected_size_vector.end());
        }*/
    };
};
