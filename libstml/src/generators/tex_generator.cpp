#include "../../include/stml.hpp"
#include "../../include/abstract_generator.hpp"
#include "../../include/stml_exception.hpp"

#include "../../include/generators/tex_generator.hpp"
#include "../../include/languages/russian/russian_language.hpp"

using namespace std;
using namespace stml;

TexGenerator::TexGenerator()
    : AbstractGenerator() {

    //TODO: Multiple languages support.
    language.reset(new RussianLanguage());
    tokenizer.reset(language->create_tokenizer());

    quotes[QUOTE_FRENCH_OPENED] = "<<";
    quotes[QUOTE_FRENCH_CLOSED] = ">>";
    quotes[QUOTE_ENGLISH_OPENED] = "``";
    quotes[QUOTE_ENGLISH_CLOSED] = "''";
    quotes[QUOTE_GERMAN_OPENED] = ",,";
    quotes[QUOTE_GERMAN_CLOSED] = "''";
    quotes[QUOTE_ENGLISH_SINGLE_OPENED] = "`";
    quotes[QUOTE_ENGLISH_SINGLE_CLOSED] = "'";

    renderers[TEX_RENDERER_CHAPTER].reset(new ChaperRenderer());
    renderers[TEX_RENDERER_SECTION].reset(new CommandRenderer("section"));
    renderers[TEX_RENDERER_SUBSECTION].reset(new CommandRenderer("subsecton"));
    renderers[TEX_RENDERER_SUBSUBSECTION].reset(new CommandRenderer("subsubsection"));
    renderers[TEX_RENDERER_PARAGRAPH].reset(new ParagraphRenderer());
    renderers[TEX_RENDERER_QUOTATION].reset(new EnvironmentRenderer("quotation"));
    renderers[TEX_RENDERER_VERSE].reset(new EnvironmentRenderer("verse"));
    renderers[TEX_RENDERER_VERBATIM].reset(new EnvironmentRenderer("verbatim"));
    renderers[TEX_RENDERER_ITEMIZE].reset(new EnvironmentRenderer("itemize"));
    renderers[TEX_RENDERER_ENUMERATE].reset(new EnvironmentRenderer("enumerate"));

    parameters[PARAM_CHAPTER_LINE_SKIP].name = L"chapter_line_skip";
    parameters[PARAM_CHAPTER_LINE_SKIP].value = "";
    parameters[PARAM_CHAPTER_SUBTITLE_FORMAT].name = L"chapter_subtitle_format";
    parameters[PARAM_CHAPTER_SUBTITLE_FORMAT].value = "";
    parameters[PARAM_CHAPTER_NUMBERS].name = L"chapter_numbers";
    parameters[PARAM_CHAPTER_NUMBERS].value = PARAM_FALSE_VALUE;
    parameters[PARAM_SECTION_NUMBERS].name = L"section_numbers";
    parameters[PARAM_SECTION_NUMBERS].value = PARAM_FALSE_VALUE;
    parameters[PARAM_SUBSECTION_NUMBERS].name = L"subsection_numbers";
    parameters[PARAM_SUBSECTION_NUMBERS].value = PARAM_FALSE_VALUE;
    parameters[PARAM_SUBSUBSECTION_NUMBERS].name = L"subsubsection_numbers";
    parameters[PARAM_SUBSUBSECTION_NUMBERS].value = PARAM_FALSE_VALUE;
    parameters[PARAM_BR_SIZE].name = L"br_size";
    parameters[PARAM_BR_SIZE].value = "10pt";
    parameters[PARAM_HR_WIDTH].name = L"hr_width";
    parameters[PARAM_HR_WIDTH].value = "100pt";
    parameters[PARAM_HR_HEIGHT].name = L"hr_height";
    parameters[PARAM_HR_HEIGHT].value = "1pt";

    continue_line = false;
    current_parameter = PARAMS_COUNT;
}

TexGenerator::~TexGenerator() {

}

void TexGenerator::TexRenderer::line(TexGenerator* generator) {
    if (generator->place_line_break) {
        *(generator->out) << "\\\\";
    }

    generator->decorate_text();
    generator->markup.write(*(generator->out));
}

void TexGenerator::ParagraphRenderer::end(TexGenerator* generator) {
    *(generator->out) << endl << endl;
}

void TexGenerator::CommandRenderer::begin(TexGenerator* generator, bool starred) {
    *(generator->out) << "\\" << command;

    if (starred) {
        *(generator->out) << "*";
    }

    *(generator->out) << "{";
}

void TexGenerator::CommandRenderer::end(TexGenerator* generator) {
    *(generator->out) << "}" << endl << endl;
}

void TexGenerator::ChaperRenderer::line(TexGenerator* generator) {
    if (generator->place_line_break) {
        *(generator->out) << "\\\\";

        string chapter_line_skip =
                generator->parameters[PARAM_CHAPTER_LINE_SKIP].as_string();
        if (!chapter_line_skip.empty()) {
            *(generator->out) << "[" << chapter_line_skip << "]";
        }

        string chapter_subtitle_format =
                generator->parameters[PARAM_CHAPTER_SUBTITLE_FORMAT].as_string();
        if (!chapter_subtitle_format.empty()) {
            *(generator->out) << chapter_subtitle_format;
        }
    }

    generator->decorate_text();
    generator->markup.write(*(generator->out));
}

void TexGenerator::EnvironmentRenderer::begin(TexGenerator* generator) {
    *(generator->out) << "\\begin{" << environment << "}" << endl << endl;
}

void TexGenerator::EnvironmentRenderer::end(TexGenerator* generator) {
    *(generator->out) << "\\end{" << environment << "}" << endl << endl;
}

void TexGenerator::document() {
    tag_stack.push(TEX_RENDERER_DOCUMENT);
    place_line_break = false;
}

void TexGenerator::header(int level) {
    TexRenderers renderer;
    bool starred;

    //If level is not specified, default is 1.
    if (level < 0) {
        level = 1;
    }

    switch (level) {
    case 1:
        renderer = TEX_RENDERER_CHAPTER;
        starred = !parameters[PARAM_CHAPTER_NUMBERS].as_boolean();
        break;
    case 2:
        renderer = TEX_RENDERER_SECTION;
        starred = !parameters[PARAM_SECTION_NUMBERS].as_boolean();
        break;
    case 3:
        renderer = TEX_RENDERER_SUBSECTION;
        starred = !parameters[PARAM_SUBSECTION_NUMBERS].as_boolean();
        break;
    case 4:
        renderer = TEX_RENDERER_SUBSUBSECTION;
        starred = !parameters[PARAM_SUBSUBSECTION_NUMBERS].as_boolean();
        break;
    default:
        throw StmlException(StmlException::UNSUPPORTED_HEADER_LEVEL);
    }

    tag_stack.push(renderer);
    ((CommandRenderer*)renderers[renderer].get())->begin(this, starred);

    place_line_break = false;
}

void TexGenerator::paragraph(Alignment alignment) {
    tag_stack.push(TEX_RENDERER_PARAGRAPH);
    place_line_break = false;
}

void TexGenerator::link(const wstring& name) {
    tag_stack.push(TEX_RENDERER_LINK);
    place_line_break = false;
}

void TexGenerator::cite(Alignment alignment) {
    tag_stack.push(TEX_RENDERER_QUOTATION);
    ((EnvironmentRenderer*)renderers[TEX_RENDERER_QUOTATION].get())->begin(this);
    place_line_break = false;
}

void TexGenerator::verse() {
    tag_stack.push(TEX_RENDERER_VERSE);
    ((EnvironmentRenderer*)renderers[TEX_RENDERER_VERSE].get())->begin(this);
    place_line_break = false;
}

void TexGenerator::preformated() {
    tag_stack.push(TEX_RENDERER_VERBATIM);
    ((EnvironmentRenderer*)renderers[TEX_RENDERER_VERBATIM].get())->begin(this);
    place_line_break = false;
}

void TexGenerator::line_break() {
    *out
    	<< "\\vspace{"
    	<< parameters[PARAM_BR_SIZE].as_string()
    	<< "}"
    	<< endl << endl;
    place_line_break = false;
}

void TexGenerator::ordered_list() {
    tag_stack.push(TEX_RENDERER_ENUMERATE);
    ((EnvironmentRenderer*)renderers[TEX_RENDERER_ENUMERATE].get())->begin(this);
    place_line_break = false;
}

void TexGenerator::unordered_list() {
    tag_stack.push(TEX_RENDERER_ITEMIZE);
    ((EnvironmentRenderer*)renderers[TEX_RENDERER_ITEMIZE].get())->begin(this);
    place_line_break = false;
}

void TexGenerator::comment() {
    tag_stack.push(TEX_RENDERER_COMMENT);
    place_line_break = false;
}

void TexGenerator::section() {
    tag_stack.push(TEX_RENDERER_STML_SECTION);
    place_line_break = false;
}

void TexGenerator::horizontal_line() {
    *out
    	<< "\\rule{"
    	<< parameters[PARAM_HR_WIDTH].as_string()
    	<< "}{"
    	<< parameters[PARAM_HR_HEIGHT].as_string()
    	<< "}"
    	<< endl << endl;
    place_line_break = false;
}

TexGenerator::Parameters TexGenerator::get_parameter_by_name(const wstring& name) const {

    for (int i = 0; i < PARAMS_COUNT; ++i) {
        if (parameters[i].name == name) {
            return (Parameters)i;
        }
    }

    return PARAM_UNKNOWN;
}

void TexGenerator::parameter(const wstring& name) {
    current_parameter = PARAM_UNKNOWN;
    if (!name.empty()) {
        current_parameter = get_parameter_by_name(name);

        if (current_parameter != PARAM_UNKNOWN) {
            parameters[current_parameter].value.clear();
        }
    }
    tag_stack.push(TEX_RENDERER_PARAMETER);
    place_line_break = false;
}

void TexGenerator::image(int width, int height, bool width_percent, bool height_percent, Alignment alignment) {

}

void TexGenerator::close_tag() {
    if (tag_stack.empty()) {
        throw StmlException(StmlException::UNEXPECTED_CLOSE_TAG);
    }

    if (!markup.empty()) {
        line_end();
    }

    TexRenderers top = tag_stack.top();

    //If there is a renderer for the tag currently on top,
    //render closing tag.
    if (renderers[top].get()) {
        renderers[top]->end(this);
    }

    tag_stack.pop();

    current_parameter = PARAMS_COUNT;
}

void TexGenerator::inject_variable(const wstring& variable_name) {

}

void TexGenerator::open_inline_tag(const wstring& tag_name) {

}

void TexGenerator::close_inline_tag() {

}

void TexGenerator::text_char(wchar_t c) {
    if (!tag_stack.empty() &&
        (tag_stack.top() == TEX_RENDERER_COMMENT || tag_stack.top() == TEX_RENDERER_DOCUMENT)) {
        return;
    }

    markup << c;

    //Parameter values are not escaped as they may have parts of TeX.
    if (current_parameter == PARAMS_COUNT) {
        if (std::char_traits<wchar_t>::find(L"#$%^&_{}~", 9, c)) {
            char escape_sequence[3] = "\\ ";
            escape_sequence[1] = c;
            markup.substitute(markup.last_char_index(), 1, escape_sequence);
        }
        else if (c == L'\\') {
            markup.substitute(markup.last_char_index(), 1, "\\textbackslash");
        }
    }
}

void TexGenerator::open_bold() {
    markup.next_char().prepend("{\\bfseries");
}

void TexGenerator::close_bold() {
    markup.last_char().append("}");
}

void TexGenerator::open_italic() {
    markup.next_char().prepend("\\empth{");
}

void TexGenerator::close_italic() {
    markup.last_char().append("}");
}

void TexGenerator::stress_mark() {
    markup.last_char().prepend("\\'");
}

void TexGenerator::line_continue() {
    continue_line = true;
}

void TexGenerator::decorate_text() {
    language->process_punctuation(markup, TEX_DASH, quotes);

    //Explicitly hyphenate words containing hyphens.
    wstring text = markup.get_text();
    tokenizer->reset();
    bool after_word = false;
    bool hyphenate_next_word = false;
    size_t word_start, word_len;

    while (tokenizer->next_token(text)) {
        Tokenizer::Token current_token = tokenizer->get_current_token();

        if (current_token.type == WORD_TOKEN) {
            word_start = current_token.start;
            word_len = current_token.length;
            after_word = true;

            if (hyphenate_next_word) {
                language->hyphenate(
                    markup,
                    word_start,
                    word_len,
                    TEX_SHY
                );
                hyphenate_next_word = false;
            }
        }
        else if (current_token.type == SINGLE_CHAR_TOKEN) {
            if (after_word && text[current_token.start] == L'-') {
                language->hyphenate(
                    markup,
                    word_start,
                    word_len,
                    TEX_SHY
                );
                hyphenate_next_word = true;
            }
            after_word = false;
        }
        else if (current_token.type == WHITESPACE_TOKEN) {
            if (after_word && hyphenate_next_word) {
                language->hyphenate(
                    markup,
                    word_start,
                    word_len,
                    TEX_SHY
                );
                hyphenate_next_word = false;
            }
            after_word = false;
        }
    }

}

void TexGenerator::line_end() {

    if (tag_stack.empty() ||
        tag_stack.top() == TEX_RENDERER_STML_SECTION ||
        tag_stack.top() == TEX_RENDERER_QUOTATION) {

        //If we are in root, section or cite,
        //render default paragraph.
        paragraph(ALIGN_DEFAULT);
        close_tag();
    }
    else {
        if (current_parameter != PARAMS_COUNT &&
            current_parameter != PARAM_UNKNOWN) {

            parameters[current_parameter].append_markup_to_value(markup);
        }
        else {
            TexRenderers top = tag_stack.top();
            //If there is a renderer for the tag currently on top,
            //render the line within tag.
            if (renderers[top].get()) {
                renderers[top]->line(this);
            }

            //If it is not specified explicitly that the line must be continued,
            //the next not empty line will start with line break.
            if (!continue_line && !markup.empty()){
                place_line_break = true;
            }
            else {
                place_line_break = false;
            }
            continue_line = false;
        }

        markup.clear();
    }
}

void TexGenerator::close_document() {

}
