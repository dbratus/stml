#include "../include/stml.hpp"
#include "../include/stml_exception.hpp"
#include "../include/parser_state.hpp"
#include "../include/abstract_generator.hpp"

using namespace std;
using namespace stml;

void AbstractParserState::init(const ParserData& parser_data) {
	m_tag_mode = parser_data.tag_mode;
}

void StartParserState::init(const ParserData& parser_data) {
	AbstractParserState::init(parser_data);
}

ParserStates StartParserState::process_char(wchar_t c,
		AbstractGeneratorPtr& generator, ParserData& parser_data) {
	if (is_tag_open(c)) {
		return PARSER_STATE_TAG;
	} else {
		if (parser_data.parse_text) {
			return PARSER_STATE_TEXT;
		} else {
			return PARSER_STATE_AS_IS_TEXT;
		}
	}
}

TagParserState::TagParserState() {
	m_tags[TAG_DOCUMENT].reset(new DocumentTag());
	m_tags[TAG_HEADER].reset(new HeaderTag());
	m_tags[TAG_PARAGRAPH].reset(new ParagraphTag());
	m_tags[TAG_LINK].reset(new LinkTag());
	m_tags[TAG_PARAMETER].reset(new ParameterTag());
	m_tags[TAG_CITE].reset(new CiteTag());
	m_tags[TAG_VERSE].reset(new VerseTag());
	m_tags[TAG_PREFORMATED].reset(new PreformatedTag());
	m_tags[TAG_LINE_BREAK].reset(new LineBreakTag());
	m_tags[TAG_ORDERED_LIST].reset(new OrderedListTag());
	m_tags[TAG_UNORDERED_LIST].reset(new UnorderedListTag());
	m_tags[TAG_COMMENT].reset(new CommentTag());
	m_tags[TAG_SECTION].reset(new SectionTag());
	m_tags[TAG_HORIZONTAL_LINE].reset(new HorizontalLineTag());
	m_tags[TAG_IMAGE].reset(new ImageTag());
}

TagParserState::Tags stml::get_tag_by_name(const wstring& tag_name) {
	if (tag_name == L"!") {
		return TagParserState::TAG_COMMENT;
	} else if (tag_name == L"doc" || tag_name == L"док") {
		return TagParserState::TAG_DOCUMENT;
	} else if (tag_name == L"h" || tag_name == L"з") {
		return TagParserState::TAG_HEADER;
	} else if (tag_name == L"p" || tag_name == L"а") {
		return TagParserState::TAG_PARAGRAPH;
	} else if (tag_name == L"link" || tag_name == L"ссылка") {
		return TagParserState::TAG_LINK;
	} else if (tag_name == L"c" || tag_name == L"ц") {
		return TagParserState::TAG_CITE;
	} else if (tag_name == L"verse" || tag_name == L"стихи") {
		return TagParserState::TAG_VERSE;
	} else if (tag_name == L"pre" || tag_name == L"преформат") {
		return TagParserState::TAG_PREFORMATED;
	} else if (tag_name == L"br" || tag_name == L"разр") {
		return TagParserState::TAG_LINE_BREAK;
	} else if (tag_name == L"ol" || tag_name == L"нс") {
		return TagParserState::TAG_ORDERED_LIST;
	} else if (tag_name == L"ul" || tag_name == L"мс") {
		return TagParserState::TAG_UNORDERED_LIST;
	} else if (tag_name == L"s" || tag_name == L"часть") {
		return TagParserState::TAG_SECTION;
	} else if (tag_name == L"hr" || tag_name == L"линия") {
		return TagParserState::TAG_HORIZONTAL_LINE;
	} else if (tag_name == L"img" || tag_name == L"рис") {
		return TagParserState::TAG_IMAGE;
	}

	return TagParserState::TAG_UNKNOWN;
}

void TagParserState::init(const ParserData& parser_data) {
	AbstractParserState::init(parser_data);

	m_current_string.clear();
	m_current_tag = TAG_UNKNOWN;
	m_closed = false;
	m_opened = false;
}

void TagParserState::init_current_tag(ParserData& parser_data) {
	if (m_current_string[0] == L'$') {
		m_current_tag = TAG_PARAMETER;
		m_tags[m_current_tag]->set_defaults();

		wstring param_name = m_current_string.substr(1,
				m_current_string.length() - 1);
		m_tags[m_current_tag]->set_arg(param_name);
		m_current_string.clear();

		parser_data.parse_text = false;
	} else {
		m_current_tag = get_tag_by_name(m_current_string);

		if (m_current_tag == TAG_UNKNOWN) {
			throw StmlException(StmlException::UNKNOWN_TAG);
		}

		m_current_string.clear();
		m_tags[m_current_tag]->set_defaults();
	}
}

ParserStates TagParserState::process_char(wchar_t c,
		AbstractGeneratorPtr& generator, ParserData& parser_data) {
	if (m_opened && !m_closed && is_tag_close(c)) {
		m_closed = true;

		if (m_current_tag == TAG_UNKNOWN && !m_current_string.empty()) {
			init_current_tag(parser_data);
		}

		//The tag without name is the closing tag.
		if (m_current_tag == TAG_UNKNOWN) {
			generator->close_tag();
		} else {
			if (!m_current_string.empty()) {
				m_tags[m_current_tag]->set_arg(m_current_string);
			}
			m_tags[m_current_tag]->commit(generator);
		}

		return PARSER_STATE_TAG;
	} else if (m_closed) {
		if (is_tag_open(c)) {
			return PARSER_STATE_INLINE_TAG;
		} else {
			if (parser_data.parse_text) {
				return PARSER_STATE_TEXT;
			} else {
				return PARSER_STATE_AS_IS_TEXT;
			}
		}
	} else {
		//If tag name is not parsed yet.
		if (m_current_tag == TAG_UNKNOWN) {
			if (is_space(c)) {//If space encountered, then the tag name has ended.
				init_current_tag(parser_data);
			} else if (is_tag_open(c)) {
				m_opened = true;
			} else {
				m_current_string += c;
			}
		} else {
			//Ignore all spaces after the tag name and within the argument list.
			if (!is_space(c)) {
				if (is_tag_arg_separator(c)) {
					m_tags[m_current_tag]->set_arg(m_current_string);
					m_current_string.clear();
				} else {
					m_current_string += c;
				}
			}
		}

		return PARSER_STATE_TAG;
	}
}

void TagParserState::HeaderTag::set_defaults() {
	m_level = -1;
}

void TagParserState::HeaderTag::set_arg(const wstring& arg) {
	size_t arg_len = arg.length();
	if (arg_len != 1 || arg[0] < L'1' || arg[0] > L'6') {
		throw StmlException(StmlException::UNSUPPORTED_HEADER_LEVEL);
	}

	swscanf(arg.c_str(), L"%d", &m_level);

	if (m_level < 1 || m_level > 9) {
		throw StmlException(StmlException::UNSUPPORTED_HEADER_LEVEL);
	}
}

void TagParserState::HeaderTag::commit(AbstractGeneratorPtr& generator) {
	generator->header(m_level);
}

void TagParserState::ParagraphTag::set_defaults() {
	m_alignment = ALIGN_DEFAULT;
}

void TagParserState::ParagraphTag::set_arg(const wstring& arg) {
	m_alignment = parse_alignment(arg);
}

void TagParserState::ParagraphTag::commit(AbstractGeneratorPtr& generator) {
	generator->paragraph(m_alignment);
}

void TagParserState::LinkTag::set_defaults() {
	m_link_name.clear();
}

void TagParserState::LinkTag::set_arg(const wstring& arg) {
	m_link_name = arg;
}

void TagParserState::LinkTag::commit(AbstractGeneratorPtr& generator) {
	generator->link(m_link_name);
}

void TagParserState::ParameterTag::set_defaults() {
	m_parameter_name.clear();
}

void TagParserState::ParameterTag::set_arg(const wstring& arg) {
	m_parameter_name = arg;
}

void TagParserState::ParameterTag::commit(AbstractGeneratorPtr& generator) {
	generator->parameter(m_parameter_name);
}

void TagParserState::CiteTag::commit(AbstractGeneratorPtr& generator) {
	generator->cite(m_alignment);
}

void TagParserState::VerseTag::commit(AbstractGeneratorPtr& generator) {
	generator->verse();
}

void TagParserState::PreformatedTag::commit(AbstractGeneratorPtr& generator) {
	generator->preformated();
}

void TagParserState::LineBreakTag::commit(AbstractGeneratorPtr& generator) {
	generator->line_break();
}

void TagParserState::OrderedListTag::commit(AbstractGeneratorPtr& generator) {
	generator->ordered_list();
}

void TagParserState::UnorderedListTag::commit(AbstractGeneratorPtr& generator) {
	generator->unordered_list();
}

void TagParserState::CommentTag::commit(AbstractGeneratorPtr& generator) {
	generator->comment();
}

void TagParserState::SectionTag::commit(AbstractGeneratorPtr& generator) {
	generator->section();
}

void TagParserState::HorizontalLineTag::commit(AbstractGeneratorPtr& generator) {
	generator->horizontal_line();
}

void TagParserState::DocumentTag::commit(AbstractGeneratorPtr& generator) {
	generator->document();
}

void TagParserState::ImageTag::set_defaults() {
	alignment = ALIGN_DEFAULT;
	width = height = 0;
	width_percent = height_percent = false;
}

void TagParserState::ImageTag::parse_size(const wstring& arg) {
	size_t len = arg.length();
	if (len == 0) {
		return;
	}

	int w = 0;
	int h = 0;
	bool wprc = false;
	bool hprc = false;

	size_t i = 0;

	if (arg[i] != L'?') {
		i = read_number(arg, 0, w);

		if (i == 0 || i == len) {
			return;
		}

		if (arg[i] == L'%') {
			wprc = true;
			++i;
		}
	}
	else {
		++i;
	}

	if (arg[i] == L'X' || arg[i] == L'x') {
		if (++i == len) {
			return;
		}
	} else {
		return;
	}


	if (arg[i] != L'?') {
		i += read_number(arg, i, h);

		if (i < len && arg[i] == L'%') {
			hprc = true;
		}
	}

	width = w;
	height = h;
	width_percent = wprc;
	height_percent = hprc;
}

void TagParserState::ImageTag::set_arg(const std::wstring& arg) {
	Alignment al = parse_alignment(arg);

	if (al != ALIGN_DEFAULT) {
		alignment = al;
	} else {
		parse_size(arg);
	}
}

void TagParserState::ImageTag::commit(AbstractGeneratorPtr& generator) {
	generator->image(width, height, width_percent, height_percent, alignment);
}

InlineTagParserState::InlineTagParserState() {
	m_tag_name.reserve(DEFAULT_TAG_NAME_CAPACITY);
}

void InlineTagParserState::init(const ParserData& parser_data) {
	AbstractParserState::init(parser_data);

	m_tag_name.clear();
	m_name_parsed = false;
	m_closed = false;
	m_opened = false;
	m_escape = false;
}

ParserStates InlineTagParserState::process_char(wchar_t c,
		AbstractGeneratorPtr& generator, ParserData& parser_data) {

	if (m_closed) {
		return PARSER_STATE_TEXT;
	}

	if (!m_escape && m_opened && m_name_parsed && is_tag_close(c)) {
		if (m_tag_name.empty()) {
			throw StmlException(StmlException::NAMELESS_INLINE_TAG);
		}

		if (!is_variable()) {
			generator->close_inline_tag();
		}

		m_closed = true;
		return PARSER_STATE_INLINE_TAG;
	}

	if (!m_name_parsed) {
		bool is_tag_c;
		if (is_space(c) || (is_tag_c = is_tag_close(c))) {
			if (m_tag_name.empty()) {
				throw StmlException(StmlException::NAMELESS_INLINE_TAG);
			}

			bool is_var = is_variable();
			if (is_var) {
				generator->inject_variable(m_tag_name.substr(1, m_tag_name.length() - 1));
			} else {
				generator->open_inline_tag(m_tag_name);
			}
			m_name_parsed = true;

			if (is_tag_c) {
				if (!is_var) {
					generator->close_inline_tag();
				}

				m_closed = true;
				return PARSER_STATE_INLINE_TAG;
			}
		} else if (is_tag_open(c)) {
			m_opened = true;
		} else {
			m_tag_name += c;
		}
	} else {
		if (!m_escape && is_escape(c)) {
			m_escape = true;
		} else if (!is_tag_close(c)) {
			generator->text_char(c);
			m_escape = false;
		}
	}

	return PARSER_STATE_INLINE_TAG;
}

void TextParserState::init(const ParserData& parser_data) {
	AbstractParserState::init(parser_data);

	m_escape = false;
	m_ignore_line_continue = parser_data.is_tag_line;
}

ParserStates TextParserState::process_char(wchar_t c,
		AbstractGeneratorPtr& generator, ParserData& parser_data) {
	if (m_escape) {
		generator->text_char(c);
		m_escape = false;
	} else if (is_escape(c)) {
		m_escape = true;
	} else if (is_tag_open(c)) {
		return PARSER_STATE_INLINE_TAG;
	} else if (is_line_continue(c)) {
		if (!m_ignore_line_continue) {
			generator->line_continue();
		} else {
			generator->text_char(c);
		}
	} else if (is_bold_open(c)) {
		generator->open_bold();
	} else if (is_italic_open(c)) {
		generator->open_italic();
	} else if (is_bold_close(c)) {
		generator->close_bold();
	} else if (is_italic_close(c)) {
		generator->close_italic();
	} else if (is_stress_mark(c)) {
		generator->stress_mark();
	} else {
		generator->text_char(c);
	}

	return PARSER_STATE_TEXT;
}

void AsIsTextParserState::init(const ParserData& parser_data) {
	AbstractParserState::init(parser_data);
}

ParserStates AsIsTextParserState::process_char(wchar_t c,
		AbstractGeneratorPtr& generator, ParserData& parser_data) {
	generator->text_char(c);
	return PARSER_STATE_AS_IS_TEXT;
}
