/* ./include/noname24/blockybinary.hpp
 * NoName24 - 2026
 */
#ifndef NONAME24_BLOCKYBINARY_HPP
#define NONAME24_BLOCKYBINARY_HPP

#include <cstdint> // u/intX_t
#include <string> // std::string
#include <vector> // std::vector
#include <span> // std::span
#include <memory> // std::X_ptr

// Сторонние библиотеки
#include <xxhash.h> // https://github.com/Cyan4973/xxHash

namespace NoName24 {
    namespace BlockyBinary {
/*  - Задумка блоков
 *  Блок состоит из частей, который делится лишь смещениями
 *  Сдвиги побайтовые, битовый сдвиги не используются
 *  [обязательные настройки] [название - uint16_t] [название] [данные - uint64_t] [данные] [xxh3 (все предыдущие блоки вместе взятые)]
 *
 *  [обязательные настройки] содержит в себе:
 *      1. Количество вложенных блоков, помещенные в данные ПЕРЕД сырыми байтами (uint32_t)
 *      2. Какой тип сжатия данных использовать (uint8_t)
 *             0 - Не использовать сжатие
 *             1 - Deflate (https://github.com/richgel999/miniz)
 *      3. Настройки для Deflate (обязательно для указания если тип сжатия данных - 1)
 *      4. Сколько битов использовать для xxh3 (0 - 64 / 1 - 128) (uint8_t)
 */

        struct BlockBaseSettings {
            // 1
            uint32_t block_number{};

            // 2
            uint8_t compression_type{};

            // 3

            // 4
            uint8_t xxh3_bit{};

            std::vector<uint8_t> dump();
        };
        struct Block {
            BlockBaseSettings base_settings; // обязательные настройки
            std::string name; // название

            // данные
            std::vector<Block> data_blocks; // на основе block_number в base_settings
            std::vector<uint8_t> data_main; // всё что после data_blocks в упакованных данных

            // xxh3
            uint64_t xxh3;
            XXH128_hash_t xxh128;

            size_t get_selfsize();
            void parse(std::span<const uint8_t> data);
            std::vector<uint8_t> dump();

            Block(BlockBaseSettings base_settings, std::string name): base_settings(base_settings), name(name)
            {}
        };
    };
};

#endif // NONAME24_BLOCKYBINARY_HPP
