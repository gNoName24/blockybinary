#include <noname24/blockybinary.hpp>

#include <string> // std::string
#include <fstream>

namespace BlockyBinary = NoName24::BlockyBinary;

std::string filename = "test.bin";

int main(int argc, char **argv) {
    std::ofstream file(filename, std::ios::binary);

    BlockyBinary::BlockBaseSettings blocksettings(0, 1, 1);
    BlockyBinary::Block mainblock(blocksettings, "testingblock");
    mainblock.data_main = std::vector<uint8_t>(
        std::string_view("adwawawdawdawdawdadwdawdawdasdfadawdawddwadwawawdawdawdawdadwdawdawdasdfadawdawddwadwawawdawdawdawdadwdawdawdasdfadawdawddwadwawawdawdawdawdadwdawdawdasdfadawdawddwadwawawdawdawdawdadwdawdawdasdf").begin(),
        std::string_view("adwawawdawdawdawdadwdawdawdasdfadawdawddwadwawawdawdawdawdadwdawdawdasdfadawdawddwadwawawdawdawdawdadwdawdawdasdfadawdawddwadwawawdawdawdawdadwdawdawdasdfadawdawddwadwawawdawdawdawdadwdawdawdasdf").end()
    );
    mainblock.base_settings.block_number = 1;

    BlockyBinary::Block babyblock(blocksettings, "babyblock");
    babyblock.data_main = std::vector<uint8_t>(
        std::string_view("this is baby blockthis is baby blockthis is baby blockthis is baby block").begin(),
        std::string_view("this is baby blockthis is baby blockthis is baby blockthis is baby block").end()
    );
    babyblock.base_settings.compression_type_1_level = 0;

    mainblock.data_blocks.push_back(babyblock);

    std::vector<unsigned char> mainblock_dump = mainblock.dump();
    file.write(reinterpret_cast<const char*>(mainblock_dump.data()), mainblock_dump.size());

    file.close();
    return 0;
}
