#include <noname24/blockyarchiver.hpp>
namespace BlockyArchiver = NoName24::BlockyArchiver;

#include <string> // std::string
#include <iostream> // std::cout
#include <filesystem> // std::filesystem
#include <stdexcept> // std::runtime_error

void help() {
    std::cout << "help:" << std::endl;
    std::cout << "    pack <file_path> [files/directories]" << std::endl;
    std::cout << "    unpack <file_path> <output_path>" << std::endl;
}

int main(int argc, char **argv) {
    if(argc > 1) {
        std::string arg = argv[1];
        if(arg == "pack") {
            if(argc > 2) {
                std::filesystem::path file_path(argv[2]);
                std::vector<std::filesystem::path> pack_paths;
                pack_paths.reserve(argc - 3);
                for(int i = 3; i < argc; i++) {
                    pack_paths.push_back(argv[i]);
                }
                BlockyBinary::BlockSettings settings;
                settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModule_XXH3>());
                BlockyBinary::BlockSettingsModule_XXH3* XXH3_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_XXH3*>(settings.modules[0].get());
                XXH3_ptr->xxh3_bit = 0;
                settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModule_CompressDeflate>());
                BlockyBinary::BlockSettingsModule_CompressDeflate* CompressDeflate_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_CompressDeflate*>(settings.modules[1].get());

                BlockyArchiver::pack(file_path, pack_paths, settings);
            } else {
                throw std::runtime_error("<file_path> не указан");
            }
        } else
        if(arg == "unpack") {
            std::filesystem::path file_path(argv[2]);
            if(argc > 3) {
                std::filesystem::path output_path(argv[3]);
                BlockyArchiver::unpack(file_path, output_path);
            } else {
                throw std::runtime_error("<output_path> не указан");
            }
        } else {
            help();
        }
    } else {
        help();
    }
    return 0;
}
