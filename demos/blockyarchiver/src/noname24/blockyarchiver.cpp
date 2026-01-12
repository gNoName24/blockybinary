/*     ./src/noname24/blockyarchiver.cpp
 * NoName24 - 2026
 */
#include <noname24/blockyarchiver.hpp>

#include <stdexcept> // std::runtime_error
#include <fstream> // std::ofstream
#include <iostream>

namespace NoName24 {
    namespace BlockyArchiver {
        void pack(std::filesystem::path file_path, std::span<std::filesystem::path> pack_paths) {
            BlockyBinary::BlockSettings settings;
            pack(file_path, pack_paths, settings);
        }
        void pack(std::filesystem::path file_path, std::span<std::filesystem::path> pack_paths, BlockyBinary::BlockSettings& block_settings) {
            BlockyBinary::Block block_archive(block_settings, "root");
            for(int i = 0; i < pack_paths.size(); i++) {
                BlockyBinary::Block block_next = pack_block(pack_paths[i], block_settings);
                block_archive.add_block(block_next);
            }

            std::ofstream file(file_path);

            std::cout << "dump" << std::endl;
            std::vector<unsigned char> block_archive_dump = block_archive.dump();
            file.write(reinterpret_cast<const char*>(block_archive_dump.data()), block_archive_dump.size());

            file.close();
        }
        BlockyBinary::Block pack_block(std::filesystem::path pack_path, BlockyBinary::BlockSettings& block_settings) {
            if(!std::filesystem::exists(pack_path)) throw std::runtime_error(pack_path.string() + " не существует");
            BlockyBinary::Block block(block_settings, pack_path.string());

            if(std::filesystem::is_regular_file(pack_path)) {
                std::cout << "pack: " << pack_path.string() << std::endl;
                block.settings.block_number = 0;
                block.settings.compression_type = 0;

                std::ifstream file(pack_path);
                block.data_main = std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                file.close();
            } else {
                std::vector<std::filesystem::path> pack_paths;
                for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(pack_path)) {
                    pack_paths.push_back(entry.path());
                }
                block.settings.block_number = pack_paths.size();
                block.settings.compression_type = 0;

                for(int i = 0; i < pack_paths.size(); i++) {
                    BlockyBinary::Block block_next = pack_block(pack_paths[i], block_settings);
                    block.add_block(block_next);
                }
            }

            return block;
        }

        void unpack(const std::filesystem::path& file_path, const std::filesystem::path& output_path) {
            if(!std::filesystem::exists(file_path)) throw std::runtime_error(file_path.string() + " не существует");
            if(!std::filesystem::is_regular_file(file_path)) throw std::runtime_error(file_path.string() + " не файл");
            if(std::filesystem::is_regular_file(output_path)) throw std::runtime_error(output_path.string() + " файл");
            std::filesystem::create_directories(output_path);
            std::ifstream file(file_path);
            BlockyBinary::Block block_archive;

            std::cout << "parse" << std::endl;
            block_archive.parse(std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));

            file.close();

            for(int i = 0; i < block_archive.settings.block_number; i++) {
                unpack_block(block_archive.data_blocks[i], output_path);
            }
        }
        void unpack_block(const BlockyBinary::Block& block, const std::filesystem::path& output_path) {
            if(block.settings.block_number == 0) { // file
                std::filesystem::path path(output_path / block.name);
                std::cout << "unpack: " << path.string() << std::endl;
                std::filesystem::path path_directory(output_path / block.name);
                std::filesystem::create_directories(path_directory.remove_filename());
                std::ofstream file(path);
                file.write(reinterpret_cast<const char*>(block.data_main.data()), block.data_main.size());
                file.close();
            } else { // directory
                for(int i = 0; i < block.settings.block_number; i++) {
                    unpack_block(block.data_blocks[i], output_path);
                }
            }
        }
    };
};
