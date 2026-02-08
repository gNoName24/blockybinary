/* ./src/noname24/blockybinary/Debug.cpp
 * noname24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

#include <fstream> // std::ifstream ; std::ofstream

namespace noname24 {
    namespace BlockyBinary {
        Debug::Debug(std::ofstream& file) : file(file) {
            this->file << std::unitbuf;
        }

        void Debug::print_offset_add(const size_t& offset_new, const size_t& offset_add) {
            print("offset: ", offset_new, " (+", offset_add, ")");
        }
        void Debug::print_span(std::span<const uint8_t> span) {
            ++depth;
            size_t width = 16;
            std::string line_hex;
            line_hex.reserve(width * 3);
            for(size_t i = 0; i < span.size(); i++) {
                line_hex += IntHelper::uint8_to_hex(span[i]);
                line_hex += ' ';

                if((i + 1) % width == 0 || i + 1 == span.size()) {
                    print(line_hex);
                    line_hex.clear();
                }
            }
            std::string line_symbols;
            line_symbols.reserve(width * 3);
            for(size_t i = 0; i < span.size(); i++) {
                if(span[i] >= 32 && span[i] <= 126) {
                    line_symbols += static_cast<char>(span[i]);
                } else {
                    line_symbols += '.';
                }
                line_symbols += "  ";

                if((i + 1) % width == 0 || i + 1 == span.size()) {
                    print(line_symbols);
                    line_symbols.clear();
                }
            }
            --depth;
        }
        void Debug::print_begin(std::string name) {
            print("+ ", name, ":");
            ++depth;
        }
            void Debug::print_section_begin(const size_t& offset) {
                print("+ section / offset: ", offset);
                ++depth;
            }
            void Debug::print_section_end(const size_t& offset) {
                --depth;
                print("- section / offset: ", offset);
            }
            void Debug::print_modules_begin(const size_t& offset) {
                print("+ modules / offset: ", offset);
                ++depth;
            }
                void Debug::print_modules_module_begin(const size_t& offset, BlockSettingsModuleBase* module, const size_t& index) {
                    print("+ ", module->name, "[", index, "] / offset: ", offset);
                    ++depth;
                }
                void Debug::print_modules_module_end(const size_t& offset, BlockSettingsModuleBase* module, const size_t& index) {
                    --depth;
                    print("- ", module->name, "[", index, "] / offset: ", offset);
                }
            void Debug::print_modules_end(const size_t& offset) {
                --depth;
                print("- modules / offset: ", offset);
            }
        void Debug::print_end(std::string name) {
            --depth;
            print("- " + name);
        }

        void Debug::print_str(const std::string& str) {
            for(size_t i = 0; i < depth; ++i)
                file << "   |";
            file << str << '\n';
        }
    };
};
