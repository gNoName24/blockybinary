/* ./include/noname24/blockyarchiver.hpp
 * NoName24 - 2026
 */
#ifndef NONAME24_BLOCKYARCHIVER_HPP
#define NONAME24_BLOCKYARCHIVER_HPP

#include <noname24/blockybinary.hpp>
namespace BlockyBinary = NoName24::BlockyBinary;

#include <filesystem> // std::filesystem
#include <span> // std::span

namespace NoName24 {
    namespace BlockyArchiver {
        void pack(std::filesystem::path file_path, std::span<std::filesystem::path> pack_paths);
        void pack(std::filesystem::path file_path, std::span<std::filesystem::path> pack_paths, BlockyBinary::BlockSettings& block_settings);
        BlockyBinary::Block pack_block(std::filesystem::path pack_path, BlockyBinary::BlockSettings& block_settings);

        void unpack(const std::filesystem::path& file_path, const std::filesystem::path& output_path);
        void unpack_block(const BlockyBinary::Block& block, const std::filesystem::path& output_path);
    };
};

#endif
