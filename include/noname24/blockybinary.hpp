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
 *      4. Сколько битов использовать для xxh3 (0 - 64 / 1 - 128) (uint8_t)
 */

        constexpr size_t XXH64_SIZE = 8;
        constexpr size_t XXH128_SIZE = 16;

        struct Block;
        struct BlockSettingsModuleBase {
            virtual std::unique_ptr<BlockSettingsModuleBase> clone() const = 0; // глубокое копирование

            virtual void dump_begin_in(
                std::vector<uint8_t>& ret
            ) {}
            virtual void dump_name_in(
                std::vector<uint8_t>& ret,
                uint16_t& name_size,
                std::array<uint8_t, 2>& name_size_array,
                std::string& name
            ) {}
            virtual void dump_data_in(
                std::vector<uint8_t>& ret,
                uint64_t& data_size,
                std::array<uint8_t, 8>& data_size_array,
                std::vector<Block> data_blocks,
                std::vector<uint8_t> data_main,
                std::vector<uint8_t>& data
            ) {}
            virtual void dump_end_in(
                std::vector<uint8_t>& ret
            ) {}
            virtual void dump_begin_out(
                std::vector<uint8_t>& ret
            ) {}
            virtual void dump_name_out(
                std::vector<uint8_t>& ret,
                uint16_t& name_size,
                std::array<uint8_t, 2>& name_size_array,
                std::string& name
            ) {}
            virtual void dump_data_out(
                std::vector<uint8_t>& ret,
                uint64_t& data_size,
                std::array<uint8_t, 8>& data_size_array,
                std::vector<Block> data_blocks,
                std::vector<uint8_t> data_main,
                std::vector<uint8_t>& data
            ) {}
            virtual void dump_end_out(
                std::vector<uint8_t>& ret
            ) {}

            virtual size_t parse(std::span<const uint8_t> data) { return 0; }
            virtual std::vector<uint8_t> dump_ret() {
                std::vector<uint8_t> ret;
                dump_to(ret);
                return ret;
            }
            virtual void dump_to(std::vector<uint8_t>& ret) {}

            virtual ~BlockSettingsModuleBase() = default;
        };

        // ----- MODULES -----
        /*  Модуль CompressDeflate
         * Использует чистый deflate (RFC 1951) - библиотека: https://github.com/richgel999/miniz
         * Что добавляет в settings:
         *     uint8_t level - уровень сжатия
         *     uint8_t strategy - стратегия сжатия
         *     uint64_t expected_size - изначальный размер данных
         * При dump: сжимаются данные
         *     Использует: level, strategy
         * При parse: расжимаются данные
         *     Использует: expected_size
         */
        struct BlockSettingsModule_CompressDeflate : BlockSettingsModuleBase {
            bool enable = true;

            uint8_t level = MZ_DEFAULT_LEVEL; // 0..9
            uint8_t strategy = MZ_DEFAULT_STRATEGY; // 0..4 (MZ_DEFAULT_STRATEGY, MZ_FILTERED, MZ_HUFFMAN_ONLY, MZ_RLE, MZ_FIXED)
            uint64_t expected_size; // АВТООПРЕДЕЛЯЕТСЯ

            void dump_data_in( // compress - in
                std::vector<uint8_t>& ret,
                uint64_t& data_size,
                std::array<uint8_t, 8>& data_size_array,
                std::vector<Block> data_blocks,
                std::vector<uint8_t> data_main,
                std::vector<uint8_t>& data
            ) override;

            void dump_to(std::vector<uint8_t>& ret) override;

            std::unique_ptr<BlockSettingsModuleBase> clone() const override {
                return std::make_unique<BlockSettingsModule_CompressDeflate>(*this);
            }
        };
        /*  Модуль XXH3
         * Использует xxHash - библиотека: https://github.com/Cyan4973/xxHash
         * Что добавляет в settings:
         *     uint8_t xxh3_bit - битность выходного XXH3 (64 / 128)
         * При dump: под конец берется ret, вычисляется XXH3 и помещается в самый конец
         *     Использует: xxh3_bit
         * При parse: под конец берется XXH3 из конца блока, вычисляется другой независимый XXH3 на основе предоставленных данных и они оба сравниваются
         *     Использует: xxh3_bit
         */
        struct BlockSettingsModule_XXH3 : BlockSettingsModuleBase {
            bool enable = true;

            uint8_t xxh3_bit = 0; // 64, 128

            void dump_end_out(
                std::vector<uint8_t>& ret
            ) override;

            void dump_to(std::vector<uint8_t>& ret) override;

            std::unique_ptr<BlockSettingsModuleBase> clone() const override {
                return std::make_unique<BlockSettingsModule_XXH3>(*this);
            }
        };
        // -------------------

        struct BlockSettings {
            // 1
            uint32_t block_number; // АВТООПРЕДЕЛЯЕТСЯ

            // 3 - модули
            std::vector<std::unique_ptr<BlockSettingsModuleBase>> modules;

            size_t get_selfsize();

            size_t parse(std::span<const uint8_t> data);
            std::vector<uint8_t> dump_ret();
            void dump_to(std::vector<uint8_t>& ret);

#if NONAME24_BLOCKYBINARY_ENABLE_PRINT
            void print(int tab);
#endif

            BlockSettings() = default;
            BlockSettings(BlockSettings&&) = default;
            BlockSettings& operator=(BlockSettings&&) = default;
            BlockSettings(const BlockSettings& other) : block_number(other.block_number) {
                modules.reserve(other.modules.size());
                for(const auto& p : other.modules) {
                    if(p) modules.push_back(p->clone());
                    else modules.push_back(nullptr);
                }
            }
            BlockSettings& operator=(const BlockSettings& other) {
                if (this == &other) return *this;
                block_number = other.block_number;
                modules.clear();

                modules.reserve(other.modules.size());
                for(const auto& p : other.modules) {
                    if(p) modules.push_back(p->clone());
                    else modules.push_back(nullptr);
                }
                return *this;
            }
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
