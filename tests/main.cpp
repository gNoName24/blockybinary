#include <noname24/blockybinary.hpp>

#include <string> // std::string
#include <fstream> // std::ifstream ; std::ofstream
#include <print> // std::print

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

void dump(BlockyBinary::BlockSettings& settings) {
    std::print("DUMP\n");
    std::ofstream file(filename, std::ios::binary);

    std::string data_main;
    int data_main_length = 1;
    for(int i = 0; i < data_main_length; i++) {
        data_main += "hello blocks! ";
    }

    BlockyBinary::Block mainblock(settings, "mainblock");
    mainblock.data_main = std::vector<uint8_t>(data_main.begin(), data_main.end());

    for(int i = 0; i < 2; i++) {
        int step = 0;
        dump_recurs(settings, mainblock, data_main, step, 2);
    }

    std::vector<uint8_t> mainblock_dump;
    mainblock.dump_to(mainblock_dump);
    file.write(reinterpret_cast<const char*>(mainblock_dump.data()), mainblock_dump.size());

    //mainblock.print(0);

    file.close();
}

void parse(BlockyBinary::BlockSettings& settings) {
    std::print("PARSE\n");

    std::ofstream debug_file("debug_file.txt");
    BlockyBinary::Debug debug(debug_file);

    std::ifstream file(filename, std::ios::binary);

    BlockyBinary::Block mainblock(settings);

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    mainblock.parse(buffer, 0, &debug);

    //mainblock.print(0);

    file.close();
}

int main(int argc, char **argv) {
    BlockyBinary::BlockSettings settings;

    //settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModule_Magic>());
    // BlockyBinary::BlockSettingsModule_Magic* Magic_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_Magic*>(settings.modules[0].get());

    //settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModule_CompressDeflate>());
    // BlockyBinary::BlockSettingsModule_CompressDeflate* CompressDeflate_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_CompressDeflate*>(settings.modules[1].get());

    //settings.modules.push_back(std::make_unique<BlockyBinary::BlockSettingsModule_XXH3>());
    // BlockyBinary::BlockSettingsModule_XXH3* XXH3_ptr = dynamic_cast<BlockyBinary::BlockSettingsModule_XXH3*>(settings.modules[0].get());

    dump(settings);
    parse(settings);

    return 0;
}
