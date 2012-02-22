#ifndef HTML_GENERATOR_H_
#define HTML_GENERATOR_H_

#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <map>

#include "../../include/markup_builder.hpp"
#include "../../include/languages/language.hpp"
#include "../../include/languages/tokenizer.hpp"
#include "../../include/variables_manager.hpp"

#define HTML_STRESS_MARK ("&acute;")
#define HTML_DASH ("&mdash;")
#define HTML_SHY ("&shy;")
#define CLASS_STRLEN (5)
#define STYLE_STRLEN (5)

namespace stml {

class HtmlGenerator: public AbstractGenerator {

	enum TagRenderers {
		TAG_RENDERER_DOCUMENT,
		TAG_RENDERER_HEADER1,
		TAG_RENDERER_HEADER2,
		TAG_RENDERER_HEADER3,
		TAG_RENDERER_HEADER4,
		TAG_RENDERER_HEADER5,
		TAG_RENDERER_HEADER6,
		TAG_RENDERER_PARAGRAPH,
		TAG_RENDERER_CITE_PARAGRAPH,
		TAG_RENDERER_CITE,
		TAG_RENDERER_VERSE,
		TAG_RENDERER_PREFORMATED,
		TAG_RENDERER_LINE_BREAK,
		TAG_RENDERER_ORDERED_LIST,
		TAG_RENDERER_UNORDERED_LIST,
		TAG_RENDERER_LINK,
		TAG_RENDERER_COMMENT,
		TAG_RENDERER_SECTION,
		TAG_RENDERER_HORIZONTAL_LINE,
		TAG_RENDERER_PARAMETER,
		TAG_RENDERER_IMAGE,
		TAG_RENDERERS_COUNT
	};

	class TagRenderer {
	protected:
		void write_attributes(std::ostream& out, const char* attr_names[],
				const char* attr_values[], size_t attr_count,
				bool exclude_class, bool exclude_style);
		void write_open(HtmlGenerator* generator, const char* tag_name,
				const char* attr_names[], const char* attr_values[],
				size_t attr_count, bool end, bool close);
		void write_close(std::ostream& out, const char* tag_name);
		virtual const char* tag_name() = 0;
		virtual var_id_t class_parameter(HtmlGenerator* generator) = 0;
		virtual var_id_t style_parameter(HtmlGenerator* generator) = 0;
	public:
		virtual void open(HtmlGenerator* generator, const char* attr_names[],
				const char* attr_values[], size_t attr_count, bool end, bool close);
		virtual void line(HtmlGenerator* generator);
		virtual void close(HtmlGenerator* generator);
	};

	typedef std::auto_ptr<TagRenderer> TagRendererPtr;

	class HeaderRenderer: public TagRenderer {
		int level;

	protected:
		const char* tag_name();
		var_id_t class_parameter(HtmlGenerator* generator);
		var_id_t style_parameter(HtmlGenerator* generator);

	public:
		HeaderRenderer(int level) :
			TagRenderer() {
			this->level = level;
		}
	};

	class ImageRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "img";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return generator->html_img_class;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return generator->html_img_style;
		}

	public:
		void line(HtmlGenerator* generator);
		void close(HtmlGenerator* generator);
	};

	class ParagraphRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "p";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return generator->html_p_class;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return generator->html_p_style;
		}
	};

	class CiteParagraphRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "p";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return generator->html_cite_p_class;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return generator->html_cite_p_style;
		}
	};

	class CiteRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "cite";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return generator->html_cite_class;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return generator->html_cite_style;
		}
		void line(HtmlGenerator* generator);
	};

	class VerseRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "p";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return generator->html_verse_class;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return generator->html_verse_style;
		}
	};

	class PreformatedRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "pre";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return generator->html_pre_class;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return generator->html_pre_style;
		}
		void line(HtmlGenerator* generator);
	};

	class LineBreakRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "br";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return UNKNOWN_VAR;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return UNKNOWN_VAR;
		}
	};

	class OrderedListRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "ol";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return generator->html_ol_class;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return generator->html_ol_style;
		}
		void line(HtmlGenerator* generator);
	};

	class UnorderedListRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "ul";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return generator->html_ul_class;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return generator->html_ul_style;
		}
		void line(HtmlGenerator* generator);
	};

	class HorizontalLineRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "hr";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return generator->html_hr_class;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return generator->html_hr_style;
		}
	};

	class SectionRenderer: public TagRenderer {
	protected:
		const char* tag_name() {
			return "div";
		}
		var_id_t class_parameter(HtmlGenerator* generator) {
			return generator->html_section_class;
		}
		var_id_t style_parameter(HtmlGenerator* generator) {
			return generator->html_section_style;
		}
		void line(HtmlGenerator* generator);
	};

	class AbstractInlineTag {
	protected:
		std::wstring name;
		std::string value;

		AbstractInlineTag(const std::wstring& name) {
			this->name = name;
		}

	public:
		~AbstractInlineTag() {
		}

		void append_markup_to_value(const MarkupBuilder& markup);

		virtual void open(HtmlGenerator* generator) = 0;
		virtual void close(HtmlGenerator* generator) = 0;
	};

	class LinkInlineTag: public AbstractInlineTag {
		std::string buffer;

	public:
		LinkInlineTag(const std::wstring& name) :
			AbstractInlineTag(name) {
		}

		void open(HtmlGenerator* generator);
		void close(HtmlGenerator* generator);
	};

	var_id_t html_no_line_breaks;
	var_id_t html_no_default_paragraphs;
	var_id_t html_no_shys;
	var_id_t html_default_p_alignment;
	var_id_t html_embedded_css;
	var_id_t html_doc_title;
	var_id_t html_body_class;
	var_id_t html_body_style;
	var_id_t html_h1_class;
	var_id_t html_h1_style;
	var_id_t html_h2_class;
	var_id_t html_h2_style;
	var_id_t html_h3_class;
	var_id_t html_h3_style;
	var_id_t html_h4_class;
	var_id_t html_h4_style;
	var_id_t html_h5_class;
	var_id_t html_h5_style;
	var_id_t html_h6_class;
	var_id_t html_h6_style;
	var_id_t html_p_class;
	var_id_t html_p_style;
	var_id_t html_cite_p_class;
	var_id_t html_cite_p_style;
	var_id_t html_cite_class;
	var_id_t html_cite_style;
	var_id_t html_verse_class;
	var_id_t html_verse_style;
	var_id_t html_pre_class;
	var_id_t html_pre_style;
	var_id_t html_ol_class;
	var_id_t html_ol_style;
	var_id_t html_ul_class;
	var_id_t html_ul_style;
	var_id_t html_link_class;
	var_id_t html_link_style;
	var_id_t html_section_class;
	var_id_t html_section_style;
	var_id_t html_hr_class;
	var_id_t html_hr_style;
	var_id_t html_img_class;
	var_id_t html_img_style;

	TagRendererPtr renderers[TAG_RENDERERS_COUNT];
	VariablesManager var;

	std::stack<TagRenderers, std::vector<TagRenderers> > tag_stack;
	std::map<std::wstring, AbstractInlineTag*> inline_tags;
	std::map<int, std::string> quotes;

	MarkupBuilder markup;
	std::auto_ptr<Language> language;
	std::auto_ptr<Tokenizer> tokenizer;

	AbstractInlineTag* current_inline_tag;
	var_id_t current_var;
	AbstractInlineTag* inline_tag_being_rednered;
	bool continue_line;
	bool place_line_break;
	bool document_opened;
	ImageTagLines image_tag_line;
	std::string image_style;

	static const char* alignment_css(stml::Alignment alignment);

	void open_aligned_tag(TagRenderers renderer, Alignment alignment);
	void generate_doc_header();

	void decorate_text();

public:
	HtmlGenerator();
	virtual ~HtmlGenerator();

	void document();
	void header(int level);
	void paragraph(Alignment alignment);
	void link(const std::wstring& name);
	void cite(Alignment alignment);
	void verse();
	void preformated();
	void line_break();
	void ordered_list();
	void unordered_list();
	void comment();
	void section();
	void horizontal_line();
	void parameter(const std::wstring& name);
	void image(int width, int height, bool width_percent, bool height_percent, Alignment alignment);

	void close_tag();
	void inject_variable(const std::wstring& variable_name);
	void open_inline_tag(const std::wstring& tag_name);
	void close_inline_tag();
	void text_char(wchar_t c);
	void open_bold();
	void close_bold();
	void open_italic();
	void close_italic();
	void line_continue();
	void line_end();
	void stress_mark();
	void close_document();
};

}

#endif /* HTML_GENERATOR_H_ */
