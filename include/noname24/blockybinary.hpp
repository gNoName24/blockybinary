/* ./include/noname24/blockybinary.hpp
 * NoName24 - 2026
 */

/* Доступные макросы:
 * NONAME24_BLOCKYBINARY_ENABLE_PRINT - Включает <print> и функции, связанные с ним (требуется C++23)
 *      Включение: add_definitions(-DNONAME24_BLOCKYBINARY_ENABLE_PRINT=1)
 */

#ifndef NONAME24_BLOCKYBINARY_HPP
#define NONAME24_BLOCKYBINARY_HPP

#ifndef NONAME24_BLOCKYBINARY_ENABLE_PRINT
    #define NONAME24_BLOCKYBINARY_ENABLE_PRINT 0
#endif

#include <cstdint> // u/intX_t
#include <string> // std::string
#include <vector> // std::vector
#include <span> // std::span
#include <memory> // std::X_ptr
#include <unordered_map> // std::unordered_map

// Сторонние библиотеки
#include <xxhash.h> // https://github.com/Cyan4973/xxHash
#include <miniz/miniz.h> // https://github.com/richgel999/miniz

namespace NoName24 {
    namespace BlockyBinary {
/*  - Задумка блоков
 *  Блок состоит из частей, который делится лишь смещениями
 *  Сдвиги побайтовые, битовый сдвиги не используются
 *  [magic] [настройки] [uint16_t / название] [uint64_t / данные] [xxh3 (все предыдущие блоки вместе взятые)]
 *
 *  [настройки] содержит в себе:
 *      1. Количество вложенных блоков, помещенные в данные ПЕРЕД сырыми байтами (uint32_t)
 *      2. Какой тип сжатия данных использовать (uint8_t)
 *             0 - Не использовать сжатие
 *             1 - Deflate (https://github.com/richgel999/miniz)
 *      ?3. Настройки для Deflate (ТОЛЬКО ЕСЛИ ТИП СЖАТИЯ - 1)
 *      4. Сколько битов использовать для xxh3 (0 - 64 / 1 - 128) (uint8_t)
 */

        constexpr size_t XXH64_SIZE = 8;
        constexpr size_t XXH128_SIZE = 16;

        struct BlockSettings_Deflate {
            static constexpr uint8_t SIZE_BYTE = 1 + 8;

            uint8_t level = 6;
            uint64_t expected_size; // АВТООПРЕДЕЛЯЕТСЯ

            std::vector<uint8_t> compress(std::span<const uint8_t> uncompressed);
            std::vector<uint8_t> uncompress(std::span<const uint8_t> compressed);

            size_t parse(std::span<const uint8_t> data);
            std::vector<uint8_t> dump_ret();
            void dump_to(std::vector<uint8_t>& ret);
        };
        struct BlockSettings {

            // 1
            uint32_t block_number; // АВТООПРЕДЕЛЯЕТСЯ

            // 2
            uint8_t compression_type = 0;

            // ?3 - Deflate
            BlockSettings_Deflate compression_type_1;

            // 4
            uint8_t xxh3_bit = 0;

            size_t get_selfsize();

            size_t parse(std::span<const uint8_t> data);
            std::vector<uint8_t> dump_ret();
            void dump_to(std::vector<uint8_t>& ret);

#if NONAME24_BLOCKYBINARY_ENABLE_PRINT
            void print(int tab);
#endif
        };
        struct Block {
            std::array<uint8_t, 8> magic_source = {'N', 'N', '2', '4', 'B', 'L', 'B', 'N'}; // def: NN24BLBN
            uint64_t magic = 0x000000000000000000; // на основе magic_source
            void magic_generate();
            void magic_generate(std::span<const uint8_t> magic_source);

            // uint64_t block_size; // АВТООПРЕДЕЛЯЕТСЯ

            BlockSettings settings;

            // имеет uint16_t name_size
            std::string name = "UNKNOWN";

            // данные
            // имеет uint64_t data_size
            std::vector<Block> data_blocks; // на основе block_number в base_settings
            std::unordered_map<std::string, size_t> data_blocks_indexed; // получение индекса блока на основе name
            std::vector<uint8_t> data_main; // всё что после data_blocks в упакованных данных

            // xxh3
            uint64_t xxh3;
            XXH128_hash_t xxh128;

            size_t get_selfsize();

            void add_block(const Block& block); // для копирования
            void add_block(Block&& block); // для перемещения

            size_t parse(std::span<const uint8_t> data);
            std::vector<uint8_t> dump_ret();
            void dump_to(std::vector<uint8_t>& ret);

#if NONAME24_BLOCKYBINARY_ENABLE_PRINT
            void print(int tab);
#endif

            Block() {
                magic_generate();
            };
            Block(const BlockSettings& settings): settings(settings) {
                magic_generate();
            };
            Block(const BlockSettings& settings, const std::string& name): settings(settings), name(name) {
                magic_generate();
            };
        };
    };
};

#endif // NONAME24_BLOCKYBINARY_HPP
