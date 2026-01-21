/* ./src/noname24/blockybinary/Block.cpp
 * NoName24 - 2026
 */
#include <noname24/blockybinary.hpp>
#include <noname24/blockybinary/inthelper.hpp>

namespace NoName24 {
    namespace BlockyBinary {
        void Block::add_block(Block block) {
            data_blocks.push_back(std::move(block));
            data_blocks_indexed[block.name] = data_blocks.size() - 1;
        }

        size_t Block::parse(std::span<const uint8_t> ret, size_t offset,
                            Debug* debug
        ) {
            /*
             * in - подготовка данных
             * out - парсинг данных в переменные блока
             */
            // -- DEBUG --
            bool debug_enable = false;
            if(debug != nullptr) {
                debug_enable = true;
                debug->print("ПАРСИНГ БЛОКА");
                debug->print("ret:");
                debug->print_span(ret);
            }
            // -----------

            if(offset > ret.size()) throw std::runtime_error("offset выходит за границы ret");

            // begin - in
            if(debug_enable) debug->print_begin("begin - in");
            if(debug_enable) debug->print_modules_begin(offset);
            for(size_t i = 0; i < settings.modules.size(); i++) { // module
                if(debug_enable) debug->print_modules_module_begin(offset, settings.modules[i].get(), i);

                if(!settings.modules[i]->enable) continue;
                offset += settings.modules[i]->parse_begin_in(ret, offset, debug);

                if(debug_enable) debug->print_modules_module_end(offset, settings.modules[i].get(), i);
            }
            if(debug_enable) debug->print_modules_end(offset);
            if(debug_enable) debug->print_end("begin - in");

            // settings - in
            if(debug_enable) debug->print_begin("settings - in");
            if(debug_enable) debug->print_section_begin(offset);

            offset += settings.parse(ret, offset);

            if(debug_enable) debug->print_section_end(offset);
            if(debug_enable) debug->print_end("settings - in");

            // name - in
            if(debug_enable) debug->print_begin("name - in");
            if(debug_enable) debug->print_section_begin(offset);

            std::span<const uint8_t> name_size_span = ret.subspan(offset, 2);
            if(debug_enable) debug->print("name_size_span: 0x",
                IntHelper::uint8_to_hex(name_size_span[1]), IntHelper::uint8_to_hex(name_size_span[0]),
                " (", IntHelper::uspan8_to_uint16(name_size_span), ")");
            std::array<uint8_t, 2> name_size_array;
            std::copy(name_size_span.begin(), name_size_span.end(), name_size_array.begin());
            offset += 2;
            if(debug_enable) debug->print_offset_add(offset, 2);

            uint16_t in_name_size = IntHelper::uspan8_to_uint16(name_size_array);
            std::span<const uint8_t> name_span = ret.subspan(offset, in_name_size);
            if(debug_enable) debug->print("name_span: ", std::string(name_span.begin(), name_span.end()));
            std::vector<uint8_t> name_vector(name_span.begin(), name_span.end());
            offset += in_name_size;
            if(debug_enable) debug->print_offset_add(offset, in_name_size);

            if(debug_enable) debug->print_section_end(offset);
            if(debug_enable) debug->print_modules_begin(offset);
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(debug_enable) debug->print_modules_module_begin(offset, settings.modules[i].get(), i);

                if(!settings.modules[i]->enable) continue;
                offset += settings.modules[i]->parse_name_in(ret, offset, name_size_array, name_vector, debug);

                if(debug_enable) debug->print_modules_module_end(offset, settings.modules[i].get(), i);
            }
            if(debug_enable) debug->print_modules_end(offset);
            if(debug_enable) debug->print_end("name - in");

            // data - in
            if(debug_enable) debug->print_begin("data - in");
            if(debug_enable) debug->print_section_begin(offset);

            std::span<const uint8_t> data_size_span = ret.subspan(offset, 8);
            if(debug_enable) debug->print("data_size_span: 0x",
                IntHelper::uint8_to_hex(data_size_span[7]), IntHelper::uint8_to_hex(data_size_span[6]),
                IntHelper::uint8_to_hex(data_size_span[5]), IntHelper::uint8_to_hex(data_size_span[4]),
                IntHelper::uint8_to_hex(data_size_span[3]), IntHelper::uint8_to_hex(data_size_span[2]),
                IntHelper::uint8_to_hex(data_size_span[1]), IntHelper::uint8_to_hex(data_size_span[0]),
                " (", IntHelper::uspan8_to_uint64(data_size_span), ")");
            std::array<uint8_t, 8> data_size_array;
            std::copy(data_size_span.begin(), data_size_span.end(), data_size_array.begin());
            offset += 8;
            if(debug_enable) debug->print_offset_add(offset, 8);

            uint64_t in_data_size = IntHelper::uspan8_to_uint64(data_size_array);
            std::span<const uint8_t> data_span = ret.subspan(offset, in_data_size);
            if(debug_enable) {
                debug->print("data_span:  ");
                debug->print_span(data_span);
            }
            std::vector<uint8_t> data_vector(data_span.begin(), data_span.end());
            offset += in_data_size;
            if(debug_enable) debug->print_offset_add(offset, in_data_size);

            if(debug_enable) debug->print_section_end(offset);
            if(debug_enable) debug->print_modules_begin(offset);
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(debug_enable) debug->print_modules_module_begin(offset, settings.modules[i].get(), i);

                if(!settings.modules[i]->enable) continue;
                offset += settings.modules[i]->parse_data_in(ret, offset, data_size_array, data_vector, debug);

                if(debug_enable) debug->print_modules_module_end(offset, settings.modules[i].get(), i);
            }
            if(debug_enable) debug->print_modules_end(offset);
            if(debug_enable) debug->print_end("data - in");

            // end - in
            if(debug_enable) debug->print_begin("end - in");
            if(debug_enable) debug->print_modules_begin(offset);
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(debug_enable) debug->print_modules_module_begin(offset, settings.modules[i].get(), i);

                if(!settings.modules[i]->enable) continue;
                offset += settings.modules[i]->parse_end_in(ret, offset, debug);

                if(debug_enable) debug->print_modules_module_end(offset, settings.modules[i].get(), i);
            }
            if(debug_enable) debug->print_modules_end(offset);
            if(debug_enable) debug->print_end("end - in");

            // begin - out
            if(debug_enable) debug->print_begin("begin - out");
            if(debug_enable) debug->print_modules_begin(offset);
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(debug_enable) debug->print_modules_module_begin(offset, settings.modules[i].get(), i);

                if(!settings.modules[i]->enable) continue;
                offset += settings.modules[i]->parse_begin_out(ret, offset, debug);

                if(debug_enable) debug->print_modules_module_end(offset, settings.modules[i].get(), i);
            }
            if(debug_enable) debug->print_modules_end(offset);
            if(debug_enable) debug->print_end("begin - out");

            // settings - out (не используется)

            // name - out
            if(debug_enable) debug->print_begin("name - out");
            uint16_t name_size = IntHelper::uspan8_to_uint16(name_size_array);
            name.assign(reinterpret_cast<const char*>(name_vector.data()), name_size);
            if(debug_enable) debug->print_modules_begin(offset);
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(debug_enable) debug->print_modules_module_begin(offset, settings.modules[i].get(), i);

                if(!settings.modules[i]->enable) continue;
                offset += settings.modules[i]->parse_name_out(ret, offset, name, debug);

                if(debug_enable) debug->print_modules_module_end(offset, settings.modules[i].get(), i);
            }
            if(debug_enable) debug->print_modules_end(offset);
            if(debug_enable) debug->print_end("name - out");

            // data - out
            if(debug_enable) debug->print_begin("data - out");
            if(debug_enable) debug->print_section_begin(offset);

            std::span<const uint8_t> data_vector_span = data_vector;

            size_t data_offset = 0;
            uint64_t data_size = data_vector_span.size();
            data_blocks.reserve(settings.block_number);
            for(int i = 0; i < settings.block_number; i++) {
                if(debug_enable) debug->print("data_offset: ", data_offset);

                Block data_blocks_block(settings);
                if(debug_enable) debug->depth++;
                data_offset += data_blocks_block.parse(data_vector_span.subspan(data_offset), 0, debug);
                if(debug_enable) debug->depth--;

                add_block(std::move(data_blocks_block));
            }
            uint64_t data_main_size = data_size - data_offset;
            std::span<const uint8_t> data_main_span = data_vector_span.subspan(data_offset, data_main_size);
            if(debug_enable) {
                debug->print("data_main_span:  ");
                debug->print_span(data_main_span);
            }
            data_main.assign(data_main_span.begin(), data_main_span.end());

            if(debug_enable) debug->print_section_end(offset);
            if(debug_enable) debug->print_modules_begin(offset);
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(debug_enable) debug->print_modules_module_begin(offset, settings.modules[i].get(), i);

                if(!settings.modules[i]->enable) continue;
                offset += settings.modules[i]->parse_data_out(ret, offset, data_blocks, data_main, debug);

                if(debug_enable) debug->print_modules_module_end(offset, settings.modules[i].get(), i);
            }
            if(debug_enable) debug->print_modules_end(offset);
            if(debug_enable) debug->print_end("data - out");

            // end - out
            if(debug_enable) debug->print_begin("end - out");
            if(debug_enable) debug->print_modules_begin(offset);
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(debug_enable) debug->print_modules_module_begin(offset, settings.modules[i].get(), i);

                if(!settings.modules[i]->enable) continue;
                offset += settings.modules[i]->parse_end_out(ret, offset, debug);

                if(debug_enable) debug->print_modules_module_end(offset, settings.modules[i].get(), i);
            }
            if(debug_enable) debug->print_modules_end(offset);
            if(debug_enable) debug->print_end("end - out");

            return offset;
        }

        std::vector<uint8_t> Block::dump_ret() {
            std::vector<uint8_t> ret;
            dump_to(ret);
            return ret;
        }
        void Block::dump_to(std::vector<uint8_t>& ret) {
            /*
             * in - подготовка данных
             * out - внесение данных в ret
             */

            settings.block_number = data_blocks.size();

            // begin - in
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(!settings.modules[i]->enable) continue;
                settings.modules[i]->dump_begin_in(ret);
            }

            // settings - in
            std::vector<uint8_t> settings_vector;

            // name - in
            uint16_t name_size = name.size();
            std::array<uint8_t, 2> name_size_array = IntHelper::uint16_to_uarray8(name_size);
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(!settings.modules[i]->enable) continue;
                settings.modules[i]->dump_name_in(ret, name_size, name_size_array, name);
            }

            // data - in
            std::vector<uint8_t> data;
            for(uint64_t i = 0; i < settings.block_number; i++) {
                std::vector<uint8_t> block_buffer;
                data_blocks[i].dump_to(block_buffer);

                data.insert(data.end(), block_buffer.begin(), block_buffer.end());
            }
            data.insert(data.end(), data_main.begin(), data_main.end());

            uint64_t data_size = data.size();
            std::array<uint8_t, 8> data_size_array = IntHelper::uint64_to_uarray8(data_size);
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(!settings.modules[i]->enable) continue;
                settings.modules[i]->dump_data_in(ret, data_size, data_size_array, data_blocks, data_main, data);
            }

            // end - in
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(!settings.modules[i]->enable) continue;
                settings.modules[i]->dump_end_in(ret);
            }

            // begin - out
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(!settings.modules[i]->enable) continue;
                settings.modules[i]->dump_begin_out(ret);
            }

            // settings - out
            settings.dump_to(settings_vector);
            ret.insert(ret.end(), settings_vector.begin(), settings_vector.end());

            // name - out
            ret.insert(ret.end(), name_size_array.begin(), name_size_array.end());
            ret.insert(ret.end(), name.begin(), name.end());
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(!settings.modules[i]->enable) continue;
                settings.modules[i]->dump_name_out(ret, name_size, name_size_array, name);
            }

            // data - out
            ret.insert(ret.end(), data_size_array.begin(), data_size_array.end());
            ret.insert(ret.end(), data.begin(), data.end());
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(!settings.modules[i]->enable) continue;
                settings.modules[i]->dump_data_out(ret, data_size, data_size_array, data_blocks, data_main, data);
            }

            // end - out
            for(int i = 0; i < settings.modules.size(); i++) { // module
                if(!settings.modules[i]->enable) continue;
                settings.modules[i]->dump_end_out(ret);
            }
        }
    };
};
