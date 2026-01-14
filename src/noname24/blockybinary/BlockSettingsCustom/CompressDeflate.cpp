/*     ./src/noname24/BlockSettingsCustom/CompressDeflate.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

namespace NoName24 {
    namespace BlockyBinary {
        void BlockSettingsModule_CompressDeflate::dump_data_in(
            std::vector<uint8_t>& ret,
            uint64_t& data_size,
            std::array<uint8_t, 8>& data_size_array,
            std::vector<Block> data_blocks,
            std::vector<uint8_t> data_main,
            std::vector<uint8_t>& data
        ) {
            if(!enable) return;
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
                throw std::runtime_error("deflate failed (mz_deflateInit2_status != MZ_OK)");
            }

            stream.next_in = data.data();
            stream.avail_in = static_cast<unsigned int>(data.size());

            uint8_t out[1024 * 32];

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
                    throw std::runtime_error("deflate failed (mz_deflate_status != MZ_OK)");
                }
            }

            mz_deflateEnd(&stream);

            data = buffer;
            data_size = data.size();
            data_size_array = IntHelper::uint64_to_uarray8(data_size);
        }

        void BlockSettingsModule_CompressDeflate::dump_to(std::vector<uint8_t>& ret) {
            // enable
            ret.push_back(enable);

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
