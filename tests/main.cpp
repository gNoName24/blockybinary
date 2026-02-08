#include <noname24/blockybinary.hpp>

#include <fstream> // std::ifstream ; std::ofstream
#include <iostream> // std::iostream

namespace BlockyBinary = noname24::BlockyBinary;

std::string filename = "test.braw";

void dump(BlockyBinary::BlockSettings& settings) {
    std::cout << "DUMP" << std::endl;
    std::ofstream file(filename, std::ios::binary);

    BlockyBinary::Block mainblock(settings, "mainblock");

    std::string data_main;
    for(size_t i = 0; i < 8; ++i) {
        BlockyBinary::Block block(settings, "block");
        data_main += "this is a text. ";
        block.data_main = std::vector<uint8_t>(data_main.begin(), data_main.end());
        mainblock.data_blocks.push_back(block);
    }

    std::vector<uint8_t> mainblock_dump;
    mainblock.dump_to(mainblock_dump);
    file.write(reinterpret_cast<const char*>(mainblock_dump.data()), mainblock_dump.size());

    file.close();
}

void parse(BlockyBinary::BlockSettings& settings) {
    std::cout << "PARSE" << std::endl;
    std::ifstream file(filename, std::ios::binary);

    std::ofstream debug_file("debug_file.txt");
    BlockyBinary::Debug debug(debug_file);

    BlockyBinary::Block mainblock(settings);

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    mainblock.parse(buffer, 0, &debug);

    file.close();
}

int main(int argc, char **argv) {
    BlockyBinary::BlockSettings settings;

    settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModules::Security::Magic>());
    settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModules::CompressDecompress::DeflateInflate>());
    settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModules::Security::XXH3>());

    dump(settings);
    parse(settings);

    return 0;
}
