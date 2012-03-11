#ifndef PARSER_STATE_H_
#define PARSER_STATE_H_

#include <string>
#include <string.h>
#include <memory>

namespace stml {

enum ParserStates {
	PARSER_STATE_START,
	PARSER_STATE_TAG,
	PARSER_STATE_INLINE_TAG,
	PARSER_STATE_TEXT,
	PARSER_STATE_AS_IS_TEXT,
	PARSER_STATES_COUNT
};

inline bool is_space(wchar_t c) {
	return c == L' ';
}
inline bool is_tag_arg_separator(wchar_t c) {
	return c == L',';
}
inline bool is_escape(wchar_t c) {
	return c == L'=';
}
inline bool is_line_continue(wchar_t c) {
	return c == L'_';
}
inline bool is_bold_open(wchar_t c) {
	return c == L'[';
}
inline bool is_bold_close(wchar_t c) {
	return c == L']';
}
inline bool is_italic_open(wchar_t c) {
	return c == L'{';
}
inline bool is_italic_close(wchar_t c) {
	return c == L'}';
}
inline bool is_stress_mark(wchar_t c) {
	return c == L'\\';
}

enum TagModes {
	TAG_MODE_ANY, TAG_MODE_LTGT, TAG_MODE_SLASH
};

inline TagModes get_tag_mode_by_tag_open(wchar_t c) {
	switch (c) {
	case L'<':
		return TAG_MODE_LTGT;
	case L'/':
		return TAG_MODE_SLASH;
	default:
		return TAG_MODE_ANY;
	}
}

struct ParserData {
	TagModes tag_mode;
	bool is_tag_line;
	bool parse_text;
	bool as_is;
};

class AbstractParserState {
	TagModes tag_mode;

protected:
	inline bool is_tag_open(wchar_t c) {
		return ((tag_mode == TAG_MODE_LTGT || tag_mode == TAG_MODE_ANY) && c == L'<')
				 || ((tag_mode == TAG_MODE_SLASH || tag_mode == TAG_MODE_ANY) && c == L'/');
	}

	inline bool is_tag_close(wchar_t c) {
		return ((tag_mode == TAG_MODE_LTGT || tag_mode == TAG_MODE_ANY) && c == L'>')
				|| ((tag_mode == TAG_MODE_SLASH || tag_mode == TAG_MODE_ANY) && c == L'/');
	}

public:
	virtual ~AbstractParserState() { }

	virtual void init(const ParserData& parser_data);
	virtual ParserStates process_char(wchar_t c, AbstractGeneratorPtr& generator, ParserData& parser_data) = 0;
};

class StartParserState : public AbstractParserState {
public:
	void init(const ParserData& parser_data);
	ParserStates process_char(wchar_t c, AbstractGeneratorPtr& generator,
			ParserData& parser_data);
};

class TagParserState : public AbstractParserState {
	enum Tags {
		TAG_DOCUMENT,
		TAG_HEADER,
		TAG_PARAGRAPH,
		TAG_LINK,
		TAG_VARIABLE,
		TAG_CITE,
		TAG_VERSE,
		TAG_PREFORMATED,
		TAG_LINE_BREAK,
		TAG_ORDERED_LIST,
		TAG_UNORDERED_LIST,
		TAG_COMMENT,
		TAG_HORIZONTAL_LINE,
		TAG_SECTION,
		TAG_IMAGE,
		TAG_ORDERED_LIST_ITEM,
		TAG_UNORDERED_LIST_ITEM,
		TAG_TERMINATOR,
		TAG_AS_IS,
		TAGS_COUNT,
		TAG_UNKNOWN
	};

	static const size_t DEFAULT_TAG_NAME_LENGTH = 10;
	static const size_t MAX_ARGC = 10;

	friend Tags get_tag_by_name(const std::wstring& tag_name);

	class Tag {
	public:
		virtual void set_defaults() { }
		virtual void set_arg(const std::wstring& arg) { }
		virtual void commit(AbstractGeneratorPtr& generator) { }
	};

	typedef std::auto_ptr<Tag> AbstractTagPtr;

	class HeaderTag : public Tag {
		int level;

	public:
		void set_defaults();
		void set_arg(const std::wstring& arg);
		void commit(AbstractGeneratorPtr& generator);
	};

	class ParagraphTag : public Tag {
	protected:
		Alignments alignment;

	public:
		void set_defaults();
		void set_arg(const std::wstring& arg);
		void commit(AbstractGeneratorPtr& generator);
	};

	class LinkTag : public Tag {
		std::wstring link_name;

	public:
		void set_defaults();
		void set_arg(const std::wstring& arg);
		void commit(AbstractGeneratorPtr& generator);
	};

	class VariableTag : public Tag {
		std::wstring var_name;

	public:
		void set_defaults();
		void set_arg(const std::wstring& arg);
		void commit(AbstractGeneratorPtr& generator);
	};

	class CiteTag : public ParagraphTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class VerseTag : public Tag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class PreformatedTag : public Tag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class LineBreakTag : public Tag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class OrderedListTag : public Tag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class UnorderedListTag : public Tag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class CommentTag : public Tag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class SectionTag : public Tag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class HorizontalLineTag : public Tag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class DocumentTag : public Tag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class ImageTag : public Tag {
		Alignments alignment;
		ImageSize size;

		/**
		 * Parses size in format:
		 * <size> ::= <width>[%]x<height>[%]
		 * <width>,<height> ::= '?' | \d+
		 *
		 * @param	arg	The string which the values are parsed from.
		 */
		void parse_size(const std::wstring& arg);

	public:
		void set_defaults();
		void set_arg(const std::wstring& arg);
		void commit(AbstractGeneratorPtr& generator);
	};

	class OrderedListItemTag : public Tag {
		int level;

	public:
		void commit(AbstractGeneratorPtr& generator);

		void set_level(int level);
	};

	class UnorderedListItemTag : public Tag {
		int level;

	public:
		void commit(AbstractGeneratorPtr& generator);

		void set_level(int level);
	};

	class TerminatorTag : public Tag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	AbstractTagPtr tags[TAGS_COUNT];

	std::wstring current_string;
	Tags current_tag;
	bool closed;
	bool opened;

	static bool is_ignoring_text_parsing(Tags tag);
	static bool all_chars_equal(const std::wstring& str, wchar_t c);
	void init_current_tag(ParserData& parser_data);

public:
	TagParserState();

	void init(const ParserData& parser_data);
	ParserStates process_char(wchar_t c, AbstractGeneratorPtr& generator, ParserData& parser_data);
};

TagParserState::Tags get_tag_by_name(const std::wstring& tag_name);

class InlineTagParserState: public AbstractParserState {
	static const size_t DEFAULT_TAG_NAME_CAPACITY = 50;

	std::wstring tag_name;
	bool name_parsed;
	bool opened;
	bool closed;
	bool escape;

	inline bool is_variable() { return tag_name[0] == L'$'; }

public:
	InlineTagParserState();

	void init(const ParserData& parser_data);
	ParserStates process_char(wchar_t c, AbstractGeneratorPtr& generator, ParserData& parser_data);
};

class TextParserState: public AbstractParserState {

	bool escape;
	bool ignore_line_continue;
	int leading_spaces_cnt;
	bool non_space_encountered;

public:
	void init(const ParserData& parser_data);
	ParserStates process_char(wchar_t c, AbstractGeneratorPtr& generator, ParserData& parser_data);
};

class AsIsTextParserState: public AbstractParserState {

public:
	void init(const ParserData& parser_data);
	ParserStates process_char(wchar_t c, AbstractGeneratorPtr& generator, ParserData& parser_data);
};

}

#endif /* PARSER_STATE_H_ */
