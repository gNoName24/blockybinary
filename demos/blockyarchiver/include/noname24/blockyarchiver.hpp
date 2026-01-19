/* ./include/noname24/blockyarchiver.hpp
 * NoName24 - 2026
 */
#ifndef NONAME24_BLOCKYARCHIVER_HPP
#define NONAME24_BLOCKYARCHIVER_HPP

#include <noname24/blockybinary.hpp>
namespace BlockyBinary = NoName24::BlockyBinary;

#include <filesystem> // std::filesystem
#include <span> // std::span

#include <iostream>

namespace NoName24 {
    namespace BlockyArchiver {
        constexpr std::string standart_file_extension = ".barc";

        void pack(const std::filesystem::path& file_path, std::span<const std::filesystem::path> pack_paths);
        void pack(const std::filesystem::path& file_path, std::span<const std::filesystem::path> pack_paths,
                  BlockyBinary::BlockSettings& block_settings);
        BlockyBinary::Block pack_block(const std::filesystem::path& pack_path, BlockyBinary::BlockSettings& block_settings);

        void unpack(const std::filesystem::path& file_path, const std::filesystem::path& output_path,
                    BlockyBinary::Debug* debug = nullptr);
        void unpack(const std::filesystem::path& file_path, const std::filesystem::path& output_path,
                    BlockyBinary::BlockSettings& block_settings,
                    BlockyBinary::Debug* debug = nullptr);
        void unpack_block(const BlockyBinary::Block& block, const std::filesystem::path& output_path);

        // archive module
        /* enable false - файл
         * enable true - директория
         */
        struct BlockyArchiver : BlockyBinary::BlockSettingsModuleBase {
            std::unique_ptr<BlockSettingsModuleBase> clone() const override {
                return std::make_unique<BlockyArchiver>(*this);
            }
            BlockyArchiver() : BlockyBinary::BlockSettingsModuleBase("BlockyArchiver::BlockyArchiver") {}
        };
    };
};

#endif
