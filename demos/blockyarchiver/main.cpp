#include <noname24/blockyarchiver.hpp>
namespace BlockyArchiver = NoName24::BlockyArchiver;

#include <string> // std::string
#include <iostream> // std::print
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
                for(int i = 3; i < argc; i++) {
                    pack_paths.push_back(argv[i]);
                }
                BlockyBinary::BlockSettings settings;
                settings.compression_type = 1;
                settings.compression_type_1.level = 9;

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
