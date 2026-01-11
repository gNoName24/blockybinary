#include <noname24/blockybinary.hpp>

#include <string> // std::string
#include <fstream>

#include <print>

namespace BlockyBinary = NoName24::BlockyBinary;

std::string filename = "test.bin";

int main(int argc, char **argv) {
    // parse
    std::ifstream file(filename, std::ios::binary);

    BlockyBinary::Block mainblock;

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    mainblock.parse(buffer);

    mainblock.print(0);

    // dump
    /*
    std::ofstream file(filename, std::ios::binary);

    BlockyBinary::BlockSettings blocksettings;
    BlockyBinary::Block mainblock(blocksettings, "mainblock");
    mainblock.settings.compression_type = 1;
    std::string mainblock_data = "hello blocks! hello blocks! hello blocks! hello blocks! hello blocks!";
    mainblock.data_main =std::vector<uint8_t>(mainblock_data.begin(), mainblock_data.end());

    BlockyBinary::Block babyblock(blocksettings, "babyblock");
    babyblock.settings.compression_type = 1;
    std::string babyblock_data = "gugu gaga.. lady gaga.. gugu gaga.. lady gaga.. gugu gaga.. lady gaga.. gugu gaga.. lady gaga.. gugu gaga.. lady gaga.. gugu gaga.. lady gaga.. gugu gaga.. lady gaga..";
    babyblock.data_main =std::vector<uint8_t>(babyblock_data.begin(), babyblock_data.end());

    mainblock.data_blocks.push_back(babyblock);

    std::vector<unsigned char> mainblock_dump = mainblock.dump();
    file.write(reinterpret_cast<const char*>(mainblock_dump.data()), mainblock_dump.size());
    */

    file.close();
    return 0;
}
