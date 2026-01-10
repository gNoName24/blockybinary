#include <noname24/blockybinary.hpp>

#include <string> // std::string
#include <fstream>

namespace BlockyBinary = NoName24::BlockyBinary;

std::string filename = "test.bin";

int main(int argc, char **argv) {
    std::ofstream file(filename, std::ios::binary);

    BlockyBinary::BlockBaseSettings blocksettings(0, 0, 0);
    BlockyBinary::Block mainblock(blocksettings, "testingblock");
    mainblock.data_main = std::vector<uint8_t>(
        std::string_view("hello block").begin(),
        std::string_view("hello block").end()
    );
    mainblock.base_settings.block_number = 1;

    BlockyBinary::Block babyblock(blocksettings, "babyblock");
    babyblock.data_main = std::vector<uint8_t>(
        std::string_view("this is baby block").begin(),
        std::string_view("this is baby block").end()
    );

    mainblock.data_blocks.push_back(babyblock);

    std::vector<unsigned char> mainblock_dump = mainblock.dump();
    file.write(reinterpret_cast<const char*>(mainblock_dump.data()), mainblock_dump.size());

    file.close();
    return 0;
}
