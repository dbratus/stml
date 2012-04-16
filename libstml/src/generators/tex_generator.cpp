#include "../../include/stml.hpp"
#include "../../include/abstract_generator.hpp"
#include "../../include/stml_exception.hpp"

#include "../../include/generators/tex_generator.hpp"
#include "../../include/languages/russian/russian_language.hpp"

#include <cstdio>

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
    renderers[TEX_RENDERER_ITEMIZE].reset(new ListRenderer("itemize"));
    renderers[TEX_RENDERER_ENUMERATE].reset(new ListRenderer("enumerate"));
    renderers[TEX_RENDERER_IMAGE].reset(new CommandRenderer("includegraphics"));

    tex_chapter_line_skip = var.reset(L"tex_chapter_line_skip", L"");
    tex_chapter_subtitle_format = var.reset(L"tex_chapter_subtitle_format", L"");
    tex_chapter_numbers = var.reset(L"tex_chapter_numbers", VAR_FALSE);
    tex_section_numbers = var.reset(L"tex_section_numbers", VAR_FALSE);
    tex_subsection_numbers = var.reset(L"tex_subsection_numbers", VAR_FALSE);
    tex_subsubsection_numbers = var.reset(L"tex_subsubsection_numbers", VAR_FALSE);
    tex_br_size = var.reset(L"tex_br_size", L"10pt");
    tex_hr_width = var.reset(L"tex_hr_width", L"100pt");
    tex_hr_height = var.reset(L"tex_hr_height", L"1pt");

    continue_line = false;
    current_var = UNKNOWN_VAR;
}

TexGenerator::~TexGenerator() {
	//Do nothing.
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

void TexGenerator::CommandRenderer::begin(TexGenerator* generator, const AttributesWriter* attr_writer, bool starred) {
    *(generator->out) << "\\" << command;

    if (starred) {
        *(generator->out) << "*";
    }

    if (attr_writer) {
    	attr_writer->write_attributes(*(generator->out));
    }

    *(generator->out) << "{";
}

void TexGenerator::CommandRenderer::end(TexGenerator* generator) {
    *(generator->out) << "}" << endl << endl;
}

void TexGenerator::ChaperRenderer::line(TexGenerator* generator) {
    if (generator->place_line_break) {
        *(generator->out) << "\\\\";

        if (!generator->var[generator->tex_chapter_line_skip].empty()) {
            *(generator->out) << "[";
            generator->var[generator->tex_chapter_line_skip].markup.write(*(generator->out));
            *(generator->out) << "]";
        }

        if (!generator->var[generator->tex_chapter_subtitle_format].empty()) {
            generator->var[generator->tex_chapter_subtitle_format].markup.write(*(generator->out));
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

void TexGenerator::ListRenderer::line(TexGenerator* generator) {
	*(generator->out) << "\\item ";
	generator->markup.write(*(generator->out));
	*(generator->out) << endl;
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
        starred = var[tex_chapter_numbers].as_boolean();
        break;
    case 2:
        renderer = TEX_RENDERER_SECTION;
        starred = var[tex_section_numbers].as_boolean();
        break;
    case 3:
        renderer = TEX_RENDERER_SUBSECTION;
        starred = var[tex_subsection_numbers].as_boolean();
        break;
    case 4:
        renderer = TEX_RENDERER_SUBSUBSECTION;
        starred = var[tex_subsubsection_numbers].as_boolean();
        break;
    default:
        throw StmlException(StmlException::UNSUPPORTED_HEADER_LEVEL);
    }

    tag_stack.push(renderer);
    ((CommandRenderer*)renderers[renderer].get())->begin(this, NULL, starred);

    place_line_break = false;
}

void TexGenerator::paragraph(Alignments alignment) {
    tag_stack.push(TEX_RENDERER_PARAGRAPH);
    place_line_break = false;
}

void TexGenerator::link(const wstring& name) {
    tag_stack.push(TEX_RENDERER_LINK);
    place_line_break = false;
}

void TexGenerator::cite(Alignments alignment) {
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
    *out << "\\vspace{";
    var[tex_br_size].markup.write(*out);
    *out << "}"
    	 << endl
    	 << endl;
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
    *out << "\\rule{";
    var[tex_hr_width].markup.write(*out);
	*out << "}{";
	var[tex_hr_height].markup.write(*out);
	*out << "}" << endl << endl;
    place_line_break = false;
}

void TexGenerator::variable(const wstring& name) {
	current_var = UNKNOWN_VAR;

	if (!name.empty()) {
		current_var = var.get_by_name(name.c_str());

		if (current_var == UNKNOWN_VAR) {
			current_var = var.reset(name.c_str(), L"");
		} else {
			var[current_var].markup.clear();
		}
	}

    tag_stack.push(TEX_RENDERER_VARIABLE);
    place_line_break = false;
}

void TexGenerator::ImageAttributesWriter::set_size(ImageSize* size) {
	this->size = size;
}

void TexGenerator::ImageAttributesWriter::write_attributes(std::ostream& out) const {
	if (size->width > 0 || size->height > 0) {
		out << "[";

		char int_str[MAX_INT_SIZE];

		if (size->width > 0) {
			sprintf(int_str, "%d", size->width);
			out << "width=" << int_str << unit_str(size->width_unit);

			if (size->height > 0) {
				out << ",";
			}
		}

		if (size->height > 0) {
			sprintf(int_str, "%d", size->height);
			out << "height=" << int_str << unit_str(size->height_unit);
		}

		out << "]";
	}
}

void TexGenerator::image(ImageSize* size, Alignments alignment) {
	ImageAttributesWriter attr_writer;

	attr_writer.set_size(size);

    tag_stack.push(TEX_RENDERER_IMAGE);
    ((CommandRenderer*)renderers[TEX_RENDERER_IMAGE].get())->begin(this, &attr_writer, false);
    place_line_break = false;
}

void TexGenerator::close_tag() {
    if (tag_stack.empty()) {
        throw StmlException(StmlException::UNEXPECTED_CLOSE_TAG);
    }

    if (!markup.empty()) {
        line_end();
    }

    TexRenderers top = tag_stack.top();

    if (top != TEX_RENDERER_ITEMIZE && top != TEX_RENDERER_ENUMERATE) {
		//If there is a renderer for the tag currently on top,
		//render closing tag.
		if (renderers[top].get()) {
			renderers[top]->end(this);
		}

		tag_stack.pop();
    }

    current_var = UNKNOWN_VAR;
}

void TexGenerator::inject_variable(const wstring& variable_name) {
	try {
		markup << var[variable_name].markup;
	} catch(const out_of_range&) {
		throw StmlException(StmlException::VARIABLE_NOT_DECLARED);
	}
}

void TexGenerator::open_inline_tag(const wstring& tag_name) {
	//No any inline tag is supported for now.
}

void TexGenerator::close_inline_tag() {
	//No any inline tag is supported for now.
}

void TexGenerator::text_char(wchar_t c) {
    if (!tag_stack.empty() &&
        (tag_stack.top() == TEX_RENDERER_COMMENT || tag_stack.top() == TEX_RENDERER_DOCUMENT)) {
        return;
    }

    markup << c;

    //Variable values are not escaped as they may have parts of TeX.
    if (current_var == UNKNOWN_VAR) {
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
    markup.next_char().prepend("{\\bfseries ");
}

void TexGenerator::close_bold() {
    markup.last_char().append("}");
}

void TexGenerator::open_italic() {
    markup.next_char().prepend("{\\em ");
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
        } else if (current_token.type == SINGLE_CHAR_TOKEN) {
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
        } else if (current_token.type == WHITESPACE_TOKEN) {
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
    } else {
        if (current_var != UNKNOWN_VAR) {
            var[current_var].markup << markup;
        } else {
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
            } else {
                place_line_break = false;
            }
            continue_line = false;
        }

        markup.clear();
    }
}

void TexGenerator::close_document() {
	//Do nothing.
}

void TexGenerator::ml_list(TexRenderers renderer, int level) {
	if (level > MAX_ML_LIST_DEPTH) {
		throw StmlException(StmlException::MAX_LIST_DEPTH_EXCEEDED);
	}

	int current_level = list_items_counter.current_item_path().size();

	if (level - current_level > 1) {
		throw StmlException(StmlException::LIST_LEVEL_HOP);
	}

	if (level < current_level) {
		int pops_count = current_level - level;

		for (int i = 0; i < pops_count; ++i) {
			renderers[tag_stack.top()]->end(this);
			tag_stack.pop();
		}
	} else if (level > current_level) {
		((EnvironmentRenderer*)renderers[renderer].get())->begin(this);
		tag_stack.push(renderer);
	}

	place_line_break = false;
	list_items_counter.increment(level);
}

void TexGenerator::ordered_list_item(int level) {
	ml_list(TEX_RENDERER_ENUMERATE, level);
}

void TexGenerator::unordered_list_item(int level) {
	ml_list(TEX_RENDERER_ITEMIZE, level);
}

void TexGenerator::terminator() {
	//If the current item path is not empty,
	//than this is the end of a multilevel list.
	if (!list_items_counter.current_item_path().empty()) {
		int pops_count = list_items_counter.current_item_path().size();

		for (int i = 0; i < pops_count; ++i) {
			renderers[tag_stack.top()]->end(this);
			tag_stack.pop();
		}

		list_items_counter.reset();
	}
}
