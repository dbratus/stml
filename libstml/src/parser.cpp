#include "../include/stml.hpp"
#include "../include/abstract_generator.hpp"
#include "../include/parser_state.hpp"
#include "../include/parser.hpp"
#include "../include/input_reader.hpp"
#include "../include/stml_exception.hpp"

#include <sstream>

using namespace std;
using namespace stml;

Parser::Parser(GeneratorTypes generator_type) {
    generator.reset(create_generator(generator_type));

    states[PARSER_STATE_START].reset(new StartParserState());
    states[PARSER_STATE_TAG].reset(new TagParserState());
    states[PARSER_STATE_INLINE_TAG].reset(new InlineTagParserState());
    states[PARSER_STATE_TEXT].reset(new TextParserState());
    states[PARSER_STATE_AS_IS_TEXT].reset(new AsIsTextParserState());

    current_state = PARSER_STATE_START;
}

void Parser::parse(istream& in, ostream& out) {
    generator->set_output(&out);
    InputReader reader(&in);

    ParserData start_state_data;
    start_state_data.is_tag_line = false;
    start_state_data.tag_mode = TAG_MODE_ANY;

    ParserData data;
    unsigned int line_no = 1;

    data.tag_mode = TAG_MODE_ANY;
    data.parse_text = true;

    try {
        while(reader.next_line()) {
            wchar_t c;

            current_state = PARSER_STATE_START;
            states[current_state]->init(start_state_data);

            data.is_tag_line = false;

            while ((c = reader.next_char())) {
                ParserStates redirected_to_state = states[current_state]->process_char(c, generator, data);

                if (redirected_to_state != current_state) {
                    current_state = redirected_to_state;

                    if (current_state == PARSER_STATE_TAG) {
                        data.is_tag_line = true;
                        data.parse_text = true;
                        data.tag_mode = get_tag_mode_by_tag_open(c);
                    }

                    states[current_state]->init(data);
                    states[current_state]->process_char(c, generator, data);
                }
            }

            if (data.is_tag_line) {
                if (current_state == PARSER_STATE_TEXT || current_state == PARSER_STATE_AS_IS_TEXT) {
                    generator->close_tag();
                } else if (current_state == PARSER_STATE_INLINE_TAG) {
                    generator->close_inline_tag();
                    generator->close_tag();
                }
            }
            else {
                if (current_state == PARSER_STATE_INLINE_TAG) {
                    generator->close_inline_tag();
                }
                generator->line_end();
            }

            ++line_no;
        }
    }
    catch (StmlException& ex) {
        ex.set_line_no(line_no);
        throw;
    }
    generator->close_document();
}
