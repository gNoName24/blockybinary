#include <noname24/blockyarchiver.hpp>
namespace BlockyArchiver = noname24::BlockyArchiver;

#include <string> // std::string
#include <iostream> // std::cout
#include <filesystem> // std::filesystem
#include <stdexcept> // std::runtime_error
#include <fstream>

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

                BlockyArchiver::pack(file_path, pack_paths);
            } else {
                throw std::runtime_error("<file_path> не указан");
            }
        } else
        if(arg == "unpack") {
            std::filesystem::path file_path(argv[2]);
            if(argc > 3) {
                /*std::ofstream debug_file("debug_file.txt");
                BlockyBinary::Debug debug(debug_file);*/

                std::filesystem::path output_path(argv[3]);

                //BlockyArchiver::unpack(file_path, output_path, settings, &debug);
                BlockyArchiver::unpack(file_path, output_path);
                // debug_file.close();
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
