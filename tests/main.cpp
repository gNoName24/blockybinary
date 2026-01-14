#define NONAME24_BLOCKYBINARY_ENABLE_PRINT 1

#include <noname24/blockybinary.hpp>

#include <string> // std::string
#include <fstream>

#include <print>

namespace BlockyBinary = NoName24::BlockyBinary;

std::string filename = "test.braw";

void dump_recurs(BlockyBinary::BlockSettings& blocksettings, BlockyBinary::Block& block, std::string& data_main, int& step, int max_step) {
    step++;
    if(step >= max_step) {
        return;
    }

    data_main += "hello blocks! ";

    BlockyBinary::Block recursblock(blocksettings, "recursblock" + std::to_string(step));
    recursblock.data_main = std::vector<uint8_t>(data_main.begin(), data_main.end());

    dump_recurs(blocksettings, recursblock, data_main, step, max_step);

    block.add_block(recursblock);
}

void dump() {
    std::print("DUMP\n");
    std::ofstream file(filename, std::ios::binary);

    std::string data_main;
    int data_main_length = 4;
    for(int i = 0; i < data_main_length; i++) {
        data_main += "hello blocks! ";
    }

    BlockyBinary::BlockSettings blocksettings;
    blocksettings.compression_type = 0;
    blocksettings.compression_type_1.level = 9;
    blocksettings.xxh3_bit = 1;
    BlockyBinary::Block mainblock(blocksettings, "mainblock");
    mainblock.settings.compression_type = 1;
    mainblock.data_main = std::vector<uint8_t>(data_main.begin(), data_main.end());

    for(int i = 0; i < 4; i++) {
        int step = 0;
        dump_recurs(blocksettings, mainblock, data_main, step, 4);
    }

    std::vector<uint8_t> mainblock_dump;
    mainblock.dump_to(mainblock_dump);
    file.write(reinterpret_cast<const char*>(mainblock_dump.data()), mainblock_dump.size());

    // mainblock.print(0);

    file.close();
}

void parse() {
    std::print("PARSE\n");
    std::ifstream file(filename, std::ios::binary);

    BlockyBinary::Block mainblock;

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    mainblock.parse(buffer);

    // mainblock.print(0);

    file.close();
}

int main(int argc, char **argv) {
    dump();
    parse();

    return 0;
}
