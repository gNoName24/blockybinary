/* NOTE: НАХОДИТСЯ В РАЗРАБОТКЕ (КАК И ЭТО, ТАК И САМ BlockyBinary 1.0)
 * Написано NoName24 для BlockyBinary 1.0
 */

#pragma endian little

// BlockSettings -> Deflate
struct BlockSettings_Deflate {
    u8 level;
    u64 expected_size;
};

struct BlockSettings {
    // 1
    u32 block_number;

    // 2
    u8 compression_type;

    // ?3 - Deflate
    if(compression_type == 1) {
        BlockSettings_Deflate compression_type_1;
    }

    // 4
    u8 xxh3_bit;
};

struct Block {
    u64 magic;
    
    BlockSettings settings;

    // name
    u16 name_size;
    char name[name_size];

    // данные
    u64 data_size;
    char data_all[data_size];

    // xxh3
    if(settings.xxh3_bit == 0) {
        u64 xxh3;
    } else {
        u128 xxh128;
    }
};

u128 input_offset in;
Block block @ input_offset;