/*     ./src/noname24/blockyarchiver.cpp
 * NoName24 - 2026
 */
#include <noname24/blockyarchiver.hpp>

#include <stdexcept> // std::runtime_error
#include <fstream> // std::ifstream ; std::ofstream
#include <iostream> // std::cout

namespace NoName24 {
    namespace BlockyArchiver {
        void pack(const std::filesystem::path& file_path, std::span<const std::filesystem::path> pack_paths) {
            BlockyBinary::BlockSettings block_settings;
            block_settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModules::Security::Magic>());
            // BlockyBinary::BlockSettingsModule_Magic* Magic_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_Magic*>(settings.modules[0].get());
            block_settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModules::CompressDecompress::DeflateInflate>());
            // BlockyBinary::BlockSettingsModule_CompressDeflate* CompressDeflate_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_CompressDeflate*>(settings.modules[1].get());
            block_settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModules::Security::XXH3>());
            // BlockyBinary::BlockSettingsModule_XXH3* XXH3_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_XXH3*>(settings.modules[2].get());

            pack(file_path, pack_paths, block_settings);
        }
        void pack(const std::filesystem::path& file_path, std::span<const std::filesystem::path> pack_paths,
                  BlockyBinary::BlockSettings& block_settings) {
            if(file_path.extension() != standart_file_extension) {
                std::cout << "Рекомендуется использовать разрешение " << standart_file_extension << std::endl;
            }

            // BlockyArchiver
            block_settings.modules.insert(block_settings.modules.begin(), std::make_unique<BlockyArchiver>());

            BlockyBinary::Block block_archive(block_settings, "root");
            for(int i = 0; i < pack_paths.size(); i++) {
                BlockyBinary::Block block_next = pack_block(pack_paths[i], block_settings);
                block_archive.add_block(std::move(block_next));
            }

            std::ofstream file(file_path, std::ios::binary);

            std::cout << "dump" << std::endl;
            std::vector<uint8_t> block_archive_dump;
            block_archive.dump_to(block_archive_dump);
            file.write(reinterpret_cast<const char*>(block_archive_dump.data()), block_archive_dump.size());

            file.close();
        }
        BlockyBinary::Block pack_block(const std::filesystem::path& pack_path, BlockyBinary::BlockSettings& block_settings) {
            if(!std::filesystem::exists(pack_path)) throw std::runtime_error(pack_path.string() + " не существует");
            BlockyBinary::Block block(block_settings, pack_path.string());
            BlockyArchiver* block_BlockyArchiver_ptr = dynamic_cast<BlockyArchiver*>(block.settings.modules[0].get());

            if(std::filesystem::is_regular_file(pack_path)) {
                std::cout << "pack: " << pack_path.string() << std::endl;
                block_BlockyArchiver_ptr->enable = false;
                block.settings.block_number = 0;

                std::ifstream file(pack_path, std::ios::binary);
                block.data_main = std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                file.close();
            } else {
                std::vector<std::filesystem::path> pack_paths;
                for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(pack_path)) {
                    pack_paths.push_back(entry.path());
                }
                block_BlockyArchiver_ptr->enable = true;
                block.settings.block_number = pack_paths.size();

                // deflate
                if(!block.settings.modules.empty() && block.settings.modules[1]) {
                    BlockyBinary::BlockSettingsModules::CompressDecompress::DeflateInflate* CompressDeflate_ptr = dynamic_cast<BlockyBinary::BlockSettingsModules::CompressDecompress::DeflateInflate*>(block.settings.modules[1].get());
                    if(CompressDeflate_ptr) {
                        CompressDeflate_ptr->enable = false;
                    }
                }

                for(int i = 0; i < pack_paths.size(); i++) {
                    block.add_block(pack_block(pack_paths[i], block_settings));
                }
            }

            return block;
        }

        void unpack(const std::filesystem::path& file_path, const std::filesystem::path& output_path,
                    BlockyBinary::Debug* debug) {
            BlockyBinary::BlockSettings block_settings;
            block_settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModules::Security::Magic>());
            // BlockyBinary::BlockSettingsModule_Magic* Magic_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_Magic*>(settings.modules[0].get());
            block_settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModules::CompressDecompress::DeflateInflate>());
            // BlockyBinary::BlockSettingsModule_CompressDeflate* CompressDeflate_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_CompressDeflate*>(settings.modules[1].get());
            block_settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModules::Security::XXH3>());
            // BlockyBinary::BlockSettingsModule_XXH3* XXH3_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_XXH3*>(settings.modules[2].get());

            unpack(file_path, output_path, block_settings, debug);
        }
        void unpack(const std::filesystem::path& file_path, const std::filesystem::path& output_path,
                    BlockyBinary::BlockSettings& block_settings,
                    BlockyBinary::Debug* debug) {
            if(!std::filesystem::exists(file_path)) throw std::runtime_error(file_path.string() + " не существует");
            if(!std::filesystem::is_regular_file(file_path)) throw std::runtime_error(file_path.string() + " не файл");
            if(std::filesystem::is_regular_file(output_path)) throw std::runtime_error(output_path.string() + " файл");
            std::filesystem::create_directories(output_path);

            // BlockyArchiver
            block_settings.modules.insert(block_settings.modules.begin(), std::make_unique<BlockyArchiver>());

            std::ifstream file(file_path);
            BlockyBinary::Block block_archive(block_settings);

            std::cout << "parse" << std::endl;
            std::vector<uint8_t> buffer(
                (std::istreambuf_iterator<char>(file)),
                 std::istreambuf_iterator<char>()
            );

            block_archive.parse(buffer, 0, debug);

            file.close();

            for(int i = 0; i < block_archive.settings.block_number; i++) {
                unpack_block(block_archive.data_blocks[i], output_path);
            }
        }
        void unpack_block(const BlockyBinary::Block& block, const std::filesystem::path& output_path) {
            BlockyArchiver* block_BlockyArchiver_ptr = dynamic_cast<BlockyArchiver*>(block.settings.modules[0].get());
            std::filesystem::path path_directory(output_path / block.name);
            if(!block_BlockyArchiver_ptr->enable) { // file
                std::filesystem::create_directories(path_directory.remove_filename());

                std::filesystem::path path(output_path / block.name);
                std::cout << "unpack: " << path.string() << std::endl;

                std::ofstream file(path);
                file.write(reinterpret_cast<const char*>(block.data_main.data()), block.data_main.size());
                file.close();
            } else { // directory
                if(block.settings.block_number == 0) {
                    std::filesystem::create_directories(path_directory);
                    std::cout << "unpack: " << path_directory.string() << "/" << std::endl;
                    return;
                }
                for(int i = 0; i < block.settings.block_number; i++) {
                    unpack_block(block.data_blocks[i], output_path);
                }
            }
        }
    };
};
