/* ./include/noname24/blockybinary.hpp
 * NoName24 - 2026
 */
#ifndef NONAME24_BLOCKYBINARY_HPP
#define NONAME24_BLOCKYBINARY_HPP

#include <cstdint> // u/intX_t
#include <memory> // std::X_ptr
#include <string> // std::string
#include <sstream> // std::ostringstream
#include <span> // std::span
#include <vector> // std::vector
#include <unordered_map> // std::unordered_map

namespace NoName24 {
    namespace BlockyBinary {
        constexpr size_t XXH64_SIZE = 8;
        constexpr size_t XXH128_SIZE = 16;

        struct Block;
        struct Debug;
        struct BlockSettingsModuleBase {
            virtual std::unique_ptr<BlockSettingsModuleBase> clone() const = 0; // глубокое копирование

            const std::string name;
            bool enable = true; // учитывается в dump и parse автоматически

            // parse - in
            virtual size_t parse_begin_in(std::span<const uint8_t> ret, size_t offset,
                                          Debug* debug = nullptr
            ) { return 0; }
            virtual size_t parse_name_in(std::span<const uint8_t> ret, size_t offset,
                                         std::array<uint8_t, 2>& name_size_array,
                                         std::vector<uint8_t>& name_vector,
                                         Debug* debug = nullptr
            ) { return 0; }
            virtual size_t parse_data_in(std::span<const uint8_t> ret, size_t offset,
                                         std::array<uint8_t, 8>& data_size_array,
                                         std::vector<uint8_t>& data_vector,
                                         Debug* debug = nullptr
            ) { return 0; }
            virtual size_t parse_end_in(std::span<const uint8_t> ret, size_t offset,
                                        Debug* debug = nullptr
            ) { return 0; }
            // parse - out
            virtual size_t parse_begin_out(std::span<const uint8_t> ret, size_t offset,
                                           Debug* debug = nullptr
            ) { return 0; } // out
            virtual size_t parse_name_out(std::span<const uint8_t> ret, size_t offset,
                                          std::string& name,
                                          Debug* debug = nullptr
            ) { return 0; }
            virtual size_t parse_data_out(std::span<const uint8_t> ret, size_t offset,
                                          std::vector<Block>& data_blocks,
                                          std::vector<uint8_t>& data_main,
                                          Debug* debug = nullptr
            ) { return 0; }
            virtual size_t parse_end_out(std::span<const uint8_t> ret, size_t offset,
                                         Debug* debug = nullptr
            ) { return 0; }

            // dump - in
            virtual void dump_begin_in(std::vector<uint8_t>& ret) {}
            virtual void dump_name_in(std::vector<uint8_t>& ret,
                                      uint16_t& name_size,
                                      std::array<uint8_t, 2>& name_size_array,
                                      std::string& name
            ) {}
            virtual void dump_data_in(std::vector<uint8_t>& ret,
                                      uint64_t& data_size,
                                      std::array<uint8_t, 8>& data_size_array,
                                      std::vector<Block>& data_blocks,
                                      std::vector<uint8_t>& data_main,
                                      std::vector<uint8_t>& data
            ) {}
            virtual void dump_end_in(std::vector<uint8_t>& ret) {}
            // dump - out
            virtual void dump_begin_out(std::vector<uint8_t>& ret) {}
            virtual void dump_name_out(std::vector<uint8_t>& ret,
                                       uint16_t& name_size,
                                       std::array<uint8_t, 2>& name_size_array,
                                       std::string& name
            ) {}
            virtual void dump_data_out(std::vector<uint8_t>& ret,
                                       uint64_t& data_size,
                                       std::array<uint8_t, 8>& data_size_array,
                                       std::vector<Block>& data_blocks,
                                       std::vector<uint8_t>& data_main,
                                       std::vector<uint8_t>& data
            ) {}
            virtual void dump_end_out(std::vector<uint8_t>& ret) {}

            virtual size_t parse(std::span<const uint8_t> ret, size_t offset,
                                 Debug* debug = nullptr
            ) { return 0; }
            virtual std::vector<uint8_t> dump_ret() {
                std::vector<uint8_t> ret;
                dump_to(ret);
                return ret;
            }
            virtual void dump_to(std::vector<uint8_t>& ret) {}

            virtual ~BlockSettingsModuleBase() = default;
            BlockSettingsModuleBase(const std::string& name = "UNKNOWN") : name(name) {}
        };

        // ----- MODULES -----
        namespace BlockSettingsModules {
            // Предоставляют защиту
            namespace Security {
                struct Magic : BlockSettingsModuleBase {
                    std::array<uint8_t, 8> magic;
                    std::array<uint8_t, 8> magic_source = {'N', 'N', '2', '4', 'B', 'L', 'B', 'N'};

                    void magic_source_to_magic(); // на основе magic_source
                    void magic_source_to_magic(std::span<const uint8_t, 8> magic_source);

                    size_t parse_begin_in(std::span<const uint8_t> ret, size_t offset,
                                                 Debug* debug = nullptr
                    ) override;
                    void dump_begin_in(std::vector<uint8_t>& ret) override;

                    std::unique_ptr<BlockSettingsModuleBase> clone() const override {
                        return std::make_unique<Magic>(*this);
                    }
                    Magic();
                };
                struct XXH3 : BlockSettingsModuleBase {
                    uint8_t xxh3_bit = 0; // 64, 128

                    size_t parse_end_out(std::span<const uint8_t> ret, size_t offset,
                                         Debug* debug = nullptr
                    ) override;
                    void dump_end_out(std::vector<uint8_t>& ret) override;

                    size_t parse(std::span<const uint8_t> ret, size_t offset,
                                 Debug* debug = nullptr
                    ) override;
                    void dump_to(std::vector<uint8_t>& ret) override;

                    std::unique_ptr<BlockSettingsModuleBase> clone() const override {
                        return std::make_unique<XXH3>(*this);
                    }
                    XXH3();
                };
            };
            // Сжимание / расжимание данных
            namespace CompressDecompress {
                struct DeflateInflate : BlockSettingsModuleBase {
                    uint8_t level; // 0..9
                    uint8_t strategy; // 0..4 (MZ_DEFAULT_STRATEGY, MZ_FILTERED, MZ_HUFFMAN_ONLY, MZ_RLE, MZ_FIXED)
                    uint64_t expected_size; // (АВТООПРЕДЕЛЯЕТСЯ)

                    // inflate
                    size_t parse_data_in(std::span<const uint8_t> ret, size_t offset,
                                         std::array<uint8_t, 8>& data_size_array,
                                         std::vector<uint8_t>& data_vector,
                                         Debug* debug = nullptr
                    ) override;
                    // deflate
                    void dump_data_in(std::vector<uint8_t>& ret,
                                      uint64_t& data_size,
                                      std::array<uint8_t, 8>& data_size_array,
                                      std::vector<Block>& data_blocks,
                                      std::vector<uint8_t>& data_main,
                                      std::vector<uint8_t>& data
                    ) override;

                    size_t parse(std::span<const uint8_t> ret, size_t offset,
                                 Debug* debug = nullptr
                    ) override;
                    void dump_to(std::vector<uint8_t>& ret) override;

                    std::unique_ptr<BlockSettingsModuleBase> clone() const override {
                        return std::make_unique<DeflateInflate>(*this);
                    }
                    DeflateInflate();
                };
            };
        };
        // -------------------

        struct BlockSettings {
            // 1
            uint32_t block_number; // (АВТООПРЕДЕЛЯЕТСЯ)

            // 3 - модули
            std::vector<std::unique_ptr<BlockSettingsModuleBase>> modules;

            size_t parse(std::span<const uint8_t> ret, size_t offset,
                         Debug* debug = nullptr
            );
            std::vector<uint8_t> dump_ret();
            void dump_to(std::vector<uint8_t>& ret);

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
                if(this == &other) return *this;
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
            // имеет uint64_t block_size (АВТООПРЕДЕЛЯЕТСЯ)

            BlockSettings settings;

            // имеет uint16_t name_size (АВТООПРЕДЕЛЯЕТСЯ)
            std::string name = "UNKNOWN";

            // данные
            // имеет uint64_t data_size (АВТООПРЕДЕЛЯЕТСЯ)
            std::vector<Block> data_blocks; // на основе block_number в settings
            std::unordered_map<std::string, size_t> data_blocks_indexed; // получение индекса блока на основе name
            std::vector<uint8_t> data_main; // всё что после data_blocks в упакованных данных

            void add_block(Block block);

            size_t parse(std::span<const uint8_t> ret, size_t offset = 0,
                         Debug* debug = nullptr
            );
            std::vector<uint8_t> dump_ret();
            void dump_to(std::vector<uint8_t>& ret);

            Block() {};
            Block(const BlockSettings& settings): settings(settings) {};
            Block(const BlockSettings& settings, const std::string& name): settings(settings), name(name) {};
        };

        // DEBUG
        struct Debug {
            std::ofstream& file;
            size_t depth = 0;

            template <typename... Args>
            void print(Args&&... args) {
                std::ostringstream oss;
                (oss << ... << args);
                print_str(oss.str());
            }

            void print_offset_add(const size_t& offset_new, const size_t& offset_add);
            void print_span(std::span<const uint8_t> span);
            void print_begin(std::string name);
                void print_section_begin(const size_t& offset);
                void print_section_end(const size_t& offset);
                void print_modules_begin(const size_t& offset);
                    void print_modules_module_begin(const size_t& offset, BlockSettingsModuleBase* module, const size_t& index);
                    void print_modules_module_end(const size_t& offset, BlockSettingsModuleBase* module, const size_t& index);
                void print_modules_end(const size_t& offset);
            void print_end(std::string name);

            Debug(std::ofstream& file);

        private:
            void print_str(const std::string& str);
        };
    };
};

#endif // NONAME24_BLOCKYBINARY_HPP
