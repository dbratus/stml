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
	return c == L' ' || c == L'\t';
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
	static const size_t DEFAULT_TAG_NAME_LENGTH = 10;
	static const size_t MAX_ARGC = 10;

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
		TAGS_COUNT,
		TAG_UNKNOWN
	};

	friend Tags get_tag_by_name(const std::wstring& tag_name);

	class AbstractTag {
	public:
		virtual void set_defaults() { }
		virtual void set_arg(const std::wstring& arg) { }
		virtual void commit(AbstractGeneratorPtr& generator) = 0;
	};

	typedef std::auto_ptr<AbstractTag> AbstractTagPtr;

	class HeaderTag : public AbstractTag {
		int level;

	public:
		void set_defaults();
		void set_arg(const std::wstring& arg);
		void commit(AbstractGeneratorPtr& generator);
	};

	class ParagraphTag : public AbstractTag {
	protected:
		Alignment alignment;

	public:
		void set_defaults();
		void set_arg(const std::wstring& arg);
		void commit(AbstractGeneratorPtr& generator);
	};

	class LinkTag : public AbstractTag {
		std::wstring link_name;

	public:
		void set_defaults();
		void set_arg(const std::wstring& arg);
		void commit(AbstractGeneratorPtr& generator);
	};

	class VariableTag : public AbstractTag {
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

	class VerseTag : public AbstractTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class PreformatedTag : public AbstractTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class LineBreakTag : public AbstractTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class OrderedListTag : public AbstractTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class UnorderedListTag : public AbstractTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class CommentTag : public AbstractTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class SectionTag : public AbstractTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class HorizontalLineTag : public AbstractTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class DocumentTag : public AbstractTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	class ImageTag : public AbstractTag {
		Alignment alignment;
		int width;
		int height;
		bool width_percent;
		bool height_percent;

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

	class OrderedListItemTag : public AbstractTag {
		int level;

	public:
		void commit(AbstractGeneratorPtr& generator);

		void set_level(int level);
	};

	class UnorderedListItemTag : public AbstractTag {
		int level;

	public:
		void commit(AbstractGeneratorPtr& generator);

		void set_level(int level);
	};

	class TerminatorTag : public AbstractTag {
	public:
		void commit(AbstractGeneratorPtr& generator);
	};

	AbstractTagPtr tags[TAGS_COUNT];

	std::wstring current_string;
	Tags current_tag;
	bool closed;
	bool opened;

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
