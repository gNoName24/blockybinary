/*     ./src/noname24/BlockBaseSettings/Deflate.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

namespace NoName24 {
    namespace BlockyBinary {
        std::vector<uint8_t> BlockSettings_Deflate::compress(std::span<const uint8_t> uncompressed) {
            std::vector<uint8_t> ret(compressBound(uncompressed.size()));

            mz_ulong compressed_len = ret.size();
            int status = mz_compress2(ret.data(), &compressed_len,
                                      uncompressed.data(), uncompressed.size(),
                                      level);
            if(status != MZ_OK) {
                throw std::runtime_error("Deflate compress failed");
            }
            ret.resize(compressed_len);

            return ret;
        }
        std::vector<uint8_t> BlockSettings_Deflate::uncompress(std::span<const uint8_t> compressed) {
            std::vector<uint8_t> ret;

            ret.resize(expected_size);
            mz_ulong dest_len = expected_size;
            int status = mz_uncompress(
                ret.data(),
                &dest_len,
                compressed.data(), compressed.size()
            );
            if(status != MZ_OK) {
                throw std::runtime_error("Deflate uncompress failed");
            }

            return ret;
        }

        size_t BlockSettings_Deflate::parse(std::span<const uint8_t> data) {
            size_t byte_shift = 0; // сдвиг

            // level
            level = data[byte_shift];
            byte_shift += 1;

            // expected_size
            std::span<const uint8_t> expected_size_span = data.subspan(byte_shift, 8);
            expected_size = IntHelper::uvec8_to_uint32(
                std::vector<uint8_t>(expected_size_span.begin(), expected_size_span.end())
            );
            byte_shift += 8;

            return byte_shift;
        }
        std::vector<uint8_t> BlockSettings_Deflate::dump() {
            std::vector<uint8_t> ret = {};

            // level
            ret.push_back(level);

            // expected_size
            std::vector<uint8_t> expected_size_vector = IntHelper::uint64_to_uvec8(expected_size);
            ret.insert(ret.end(), expected_size_vector.begin(), expected_size_vector.end());

            return ret;
        }
    };
};
