#include "../include/stml.hpp"
#include "../include/stml_exception.hpp"
#include "../include/parser_state.hpp"
#include "../include/abstract_generator.hpp"

using namespace std;
using namespace stml;

void AbstractParserState::init(const ParserData& parser_data) {
	tag_mode = parser_data.tag_mode;
}

void StartParserState::init(const ParserData& parser_data) {
	AbstractParserState::init(parser_data);
}

ParserStates StartParserState::process_char(wchar_t c, AbstractGeneratorPtr& generator, ParserData& parser_data) {
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
	tags[TAG_DOCUMENT].reset(new DocumentTag());
	tags[TAG_HEADER].reset(new HeaderTag());
	tags[TAG_PARAGRAPH].reset(new ParagraphTag());
	tags[TAG_LINK].reset(new LinkTag());
	tags[TAG_VARIABLE].reset(new VariableTag());
	tags[TAG_CITE].reset(new CiteTag());
	tags[TAG_VERSE].reset(new VerseTag());
	tags[TAG_PREFORMATED].reset(new PreformatedTag());
	tags[TAG_LINE_BREAK].reset(new LineBreakTag());
	tags[TAG_ORDERED_LIST].reset(new OrderedListTag());
	tags[TAG_UNORDERED_LIST].reset(new UnorderedListTag());
	tags[TAG_COMMENT].reset(new CommentTag());
	tags[TAG_SECTION].reset(new SectionTag());
	tags[TAG_HORIZONTAL_LINE].reset(new HorizontalLineTag());
	tags[TAG_IMAGE].reset(new ImageTag());
	tags[TAG_ORDERED_LIST_ITEM].reset(new OrderedListItemTag());
	tags[TAG_UNORDERED_LIST_ITEM].reset(new UnorderedListItemTag());
	tags[TAG_TERMINATOR].reset(new TerminatorTag());
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
	} else if (tag_name == L"br" || tag_name == L"разрыв") {
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
	} else if (tag_name == L".") {
		return TagParserState::TAG_TERMINATOR;
	}

	return TagParserState::TAG_UNKNOWN;
}

void TagParserState::init(const ParserData& parser_data) {
	AbstractParserState::init(parser_data);

	current_string.clear();
	current_tag = TAG_UNKNOWN;
	closed = false;
	opened = false;
}

bool TagParserState::all_chars_equal(const wstring& str, wchar_t c) {
	for (wstring::const_iterator chr = str.begin(); chr != str.end(); ++chr) {
		if (*chr != c) {
			return false;
		}
	}

	return true;
}

bool TagParserState::is_ignoring_text_parsing(Tags tag) {
	static const Tags tags_ignoring_parsing[] = { TAG_PREFORMATED, TAG_LINK };

	for (size_t i = 0; i < sizeof(tags_ignoring_parsing)/sizeof(Tags); ++i) {
		if (tag == tags_ignoring_parsing[i]) {
			return true;
		}
	}

	return false;
}

void TagParserState::init_current_tag(ParserData& parser_data) {
	if (current_string[0] == L'$') {
		current_tag = TAG_VARIABLE;
		tags[current_tag]->set_defaults();

		wstring param_name = current_string.substr(1, current_string.length() - 1);
		tags[current_tag]->set_arg(param_name);
		current_string.clear();

		parser_data.parse_text = false;
	} else if (all_chars_equal(current_string, L'#')) {
		current_tag = TAG_ORDERED_LIST_ITEM;
		((OrderedListItemTag*)tags[current_tag].get())->set_level(current_string.size());
		current_string.clear();
	} else if (all_chars_equal(current_string, L'*')) {
		current_tag = TAG_UNORDERED_LIST_ITEM;
		((UnorderedListItemTag*)tags[current_tag].get())->set_level(current_string.size());
		current_string.clear();
	} else {
		current_tag = get_tag_by_name(current_string);

		if (current_tag == TAG_UNKNOWN) {
			throw StmlException(StmlException::UNKNOWN_TAG);
		}

		parser_data.parse_text = !is_ignoring_text_parsing(current_tag);

		current_string.clear();
		tags[current_tag]->set_defaults();
	}
}

ParserStates TagParserState::process_char(wchar_t c, AbstractGeneratorPtr& generator, ParserData& parser_data) {
	if (opened && !closed && is_tag_close(c)) {
		closed = true;

		if (current_tag == TAG_UNKNOWN && !current_string.empty()) {
			init_current_tag(parser_data);
		}

		//The tag without name is the closing tag.
		if (current_tag == TAG_UNKNOWN) {
			generator->close_tag();
		} else {
			if (!current_string.empty()) {
				tags[current_tag]->set_arg(current_string);
			}
			tags[current_tag]->commit(generator);
		}

		return PARSER_STATE_TAG;
	} else if (closed) {
		if (is_tag_open(c) && parser_data.parse_text) {
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
		if (current_tag == TAG_UNKNOWN) {
			if (is_space(c)) {//If space encountered, then the tag name has ended.
				init_current_tag(parser_data);
			} else if (is_tag_open(c)) {
				opened = true;
			} else {
				current_string += c;
			}
		} else {
			//Ignore all spaces after the tag name and within the argument list.
			if (!is_space(c)) {
				if (is_tag_arg_separator(c)) {
					tags[current_tag]->set_arg(current_string);
					current_string.clear();
				} else {
					current_string += c;
				}
			}
		}

		return PARSER_STATE_TAG;
	}
}

void TagParserState::HeaderTag::set_defaults() {
	level = -1;
}

void TagParserState::HeaderTag::set_arg(const wstring& arg) {
	size_t arg_len = arg.length();
	if (arg_len != 1 || arg[0] < L'1' || arg[0] > L'6') {
		throw StmlException(StmlException::UNSUPPORTED_HEADER_LEVEL);
	}

	swscanf(arg.c_str(), L"%d", &level);

	if (level < 1 || level > 9) {
		throw StmlException(StmlException::UNSUPPORTED_HEADER_LEVEL);
	}
}

void TagParserState::HeaderTag::commit(AbstractGeneratorPtr& generator) {
	generator->header(level);
}

void TagParserState::ParagraphTag::set_defaults() {
	alignment = ALIGN_DEFAULT;
}

void TagParserState::ParagraphTag::set_arg(const wstring& arg) {
	alignment = parse_alignment(arg);
}

void TagParserState::ParagraphTag::commit(AbstractGeneratorPtr& generator) {
	generator->paragraph(alignment);
}

void TagParserState::LinkTag::set_defaults() {
	link_name.clear();
}

void TagParserState::LinkTag::set_arg(const wstring& arg) {
	link_name = arg;
}

void TagParserState::LinkTag::commit(AbstractGeneratorPtr& generator) {
	generator->link(link_name);
}

void TagParserState::VariableTag::set_defaults() {
	var_name.clear();
}

void TagParserState::VariableTag::set_arg(const wstring& arg) {
	var_name = arg;
}

void TagParserState::VariableTag::commit(AbstractGeneratorPtr& generator) {
	generator->variable(var_name);
}

void TagParserState::CiteTag::commit(AbstractGeneratorPtr& generator) {
	generator->cite(alignment);
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
	} else {
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

void TagParserState::OrderedListItemTag::commit(AbstractGeneratorPtr& generator) {
	generator->ordered_list_item(level);
}

void TagParserState::OrderedListItemTag::set_level(int level) {
	this->level = level;
}

void TagParserState::UnorderedListItemTag::commit(AbstractGeneratorPtr& generator) {
	generator->unordered_list_item(level);
}

void TagParserState::UnorderedListItemTag::set_level(int level) {
	this->level = level;
}

void TagParserState::TerminatorTag::commit(AbstractGeneratorPtr& generator) {
	generator->terminator();
}

InlineTagParserState::InlineTagParserState() {
	tag_name.reserve(DEFAULT_TAG_NAME_CAPACITY);
}

void InlineTagParserState::init(const ParserData& parser_data) {
	AbstractParserState::init(parser_data);

	tag_name.clear();
	name_parsed = false;
	closed = false;
	opened = false;
	escape = false;
}

ParserStates InlineTagParserState::process_char(wchar_t c, AbstractGeneratorPtr& generator, ParserData& parser_data) {
	if (closed) {
		return PARSER_STATE_TEXT;
	}

	if (!escape && opened && name_parsed && is_tag_close(c)) {
		if (tag_name.empty()) {
			throw StmlException(StmlException::NAMELESS_INLINE_TAG);
		}

		if (!is_variable()) {
			generator->close_inline_tag();
		}

		closed = true;
		return PARSER_STATE_INLINE_TAG;
	}

	if (!name_parsed) {
		bool is_tag_c;
		if (is_space(c) || (is_tag_c = is_tag_close(c))) {
			if (tag_name.empty()) {
				throw StmlException(StmlException::NAMELESS_INLINE_TAG);
			}

			bool is_var = is_variable();
			if (is_var) {
				generator->inject_variable(tag_name.substr(1, tag_name.length() - 1));
			} else {
				generator->open_inline_tag(tag_name);
			}
			name_parsed = true;

			if (is_tag_c) {
				if (!is_var) {
					generator->close_inline_tag();
				}

				closed = true;
				return PARSER_STATE_INLINE_TAG;
			}
		} else if (is_tag_open(c)) {
			opened = true;
		} else {
			tag_name += c;
		}
	} else {
		if (!escape && is_escape(c)) {
			escape = true;
		} else if (!is_tag_close(c)) {
			generator->text_char(c);
			escape = false;
		}
	}

	return PARSER_STATE_INLINE_TAG;
}

void TextParserState::init(const ParserData& parser_data) {
	AbstractParserState::init(parser_data);

	non_space_encountered = false;
	leading_spaces_cnt = 0;
	escape = false;
	ignore_line_continue = parser_data.is_tag_line;
}

ParserStates TextParserState::process_char(wchar_t c, AbstractGeneratorPtr& generator, ParserData& parser_data) {
	if (!is_space(c) && !non_space_encountered) {
		non_space_encountered = true;

		if (!is_tag_open(c)) {
			for (int i = 0; i < leading_spaces_cnt; ++i) {
				generator->text_char(L' ');
			}
		}
	}

	if (escape) {
		generator->text_char(c);
		escape = false;
	} else if (is_escape(c)) {
		escape = true;
	} else if (is_tag_open(c)) {
		return PARSER_STATE_INLINE_TAG;
	} else if (is_line_continue(c)) {
		if (!ignore_line_continue) {
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
	} else if (!non_space_encountered && is_space(c)) {
		++leading_spaces_cnt;
	} else {
		generator->text_char(c);
	}

	return PARSER_STATE_TEXT;
}

void AsIsTextParserState::init(const ParserData& parser_data) {
	AbstractParserState::init(parser_data);
}

ParserStates AsIsTextParserState::process_char(wchar_t c, AbstractGeneratorPtr& generator, ParserData& parser_data) {
	generator->text_char(c);
	return PARSER_STATE_AS_IS_TEXT;
}
