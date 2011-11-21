#include "../../include/stml.hpp"
#include "../../include/utf8.hpp"
#include "../../include/abstract_generator.hpp"
#include "../../include/stml_exception.hpp"

#include "../../include/generators/html_generator.hpp"
#include "../../include/languages/russian/russian_language.hpp"

#include <cstdio>

using namespace std;
using namespace stml;

HtmlGenerator::HtmlGenerator() :
	AbstractGenerator() {

	//TODO: Multiple languages support.
	language.reset(new RussianLanguage());
	tokenizer.reset(language->create_tokenizer());

	quotes[QUOTE_FRENCH_OPENED] = "&laquo;";
	quotes[QUOTE_FRENCH_CLOSED] = "&raquo;";
	quotes[QUOTE_ENGLISH_OPENED] = "&ldquo;";
	quotes[QUOTE_ENGLISH_CLOSED] = "&rdquo;";
	quotes[QUOTE_GERMAN_OPENED] = "&bdquo;";
	quotes[QUOTE_GERMAN_CLOSED] = "&ldquo;";
	quotes[QUOTE_ENGLISH_SINGLE_OPENED] = "&lsquo;";
	quotes[QUOTE_ENGLISH_SINGLE_CLOSED] = "&rsquo;";

	renderers[TAG_RENDERER_HEADER1].reset(new HtmlGenerator::HeaderRenderer(1));
	renderers[TAG_RENDERER_HEADER2].reset(new HtmlGenerator::HeaderRenderer(2));
	renderers[TAG_RENDERER_HEADER3].reset(new HtmlGenerator::HeaderRenderer(3));
	renderers[TAG_RENDERER_HEADER4].reset(new HtmlGenerator::HeaderRenderer(4));
	renderers[TAG_RENDERER_HEADER5].reset(new HtmlGenerator::HeaderRenderer(5));
	renderers[TAG_RENDERER_HEADER6].reset(new HtmlGenerator::HeaderRenderer(6));
	renderers[TAG_RENDERER_PARAGRAPH].reset(
			new HtmlGenerator::ParagraphRenderer());
	renderers[TAG_RENDERER_CITE_PARAGRAPH].reset(
			new HtmlGenerator::CiteParagraphRenderer());
	renderers[TAG_RENDERER_CITE].reset(new HtmlGenerator::CiteRenderer());
	renderers[TAG_RENDERER_VERSE].reset(new HtmlGenerator::VerseRenderer());
	renderers[TAG_RENDERER_PREFORMATED].reset(
			new HtmlGenerator::PreformatedRenderer());
	renderers[TAG_RENDERER_LINE_BREAK].reset(
			new HtmlGenerator::LineBreakRenderer());
	renderers[TAG_RENDERER_ORDERED_LIST].reset(
			new HtmlGenerator::OrderedListRenderer());
	renderers[TAG_RENDERER_UNORDERED_LIST].reset(
			new HtmlGenerator::UnorderedListRenderer());
	renderers[TAG_RENDERER_SECTION].reset(new HtmlGenerator::SectionRenderer());
	renderers[TAG_RENDERER_HORIZONTAL_LINE].reset(
			new HtmlGenerator::HorizontalLineRenderer());
	renderers[TAG_RENDERER_IMAGE].reset(new HtmlGenerator::ImageRenderer());

	html_no_line_breaks = var.reset(L"html_no_line_breaks", VAR_FALSE);
	html_no_default_paragraphs = var.reset(L"html_no_default_paragraphs", VAR_FALSE);
	html_no_shys = var.reset(L"html_no_shys", VAR_FALSE);
	html_default_p_alignment = var.reset(L"html_default_p_alignment", L"aj");
	html_embedded_css = var.reset(L"html_embedded_css", L"");
	html_doc_title = var.reset(L"html_doc_title", L"");
	html_body_class = var.reset(L"html_body_class", L"");
	html_body_style = var.reset(L"html_body_style", L"");
	html_h1_class = var.reset(L"html_h1_class", L"");
	html_h1_style = var.reset(L"html_h1_style", L"");
	html_h2_class = var.reset(L"html_h2_class", L"");
	html_h2_style = var.reset(L"html_h2_style", L"");
	html_h3_class = var.reset(L"html_h3_class", L"");
	html_h3_style = var.reset(L"html_h3_style", L"");
	html_h4_class = var.reset(L"html_h4_class", L"");
	html_h4_style = var.reset(L"html_h4_style", L"");
	html_h5_class = var.reset(L"html_h5_class", L"");
	html_h5_style = var.reset(L"html_h5_style", L"");
	html_h6_class = var.reset(L"html_h6_class", L"");
	html_h6_style = var.reset(L"html_h6_style", L"");
	html_p_class = var.reset(L"html_p_class", L"");
	html_p_style = var.reset(L"html_p_style", L"");
	html_cite_p_class = var.reset(L"html_cite_p_class", L"");
	html_cite_p_style = var.reset(L"html_cite_p_style", L"");
	html_cite_class = var.reset(L"html_cite_class", L"");
	html_cite_style = var.reset(L"html_cite_style", L"");
	html_verse_class = var.reset(L"html_verse_class", L"stml_verse");
	html_verse_style = var.reset(L"html_verse_style", L"");
	html_pre_class = var.reset(L"html_pre_class", L"");
	html_pre_style = var.reset(L"html_pre_style", L"");
	html_ol_class = var.reset(L"html_ol_class", L"");
	html_ol_style = var.reset(L"html_ol_style", L"");
	html_ul_class = var.reset(L"html_ul_class", L"");
	html_ul_style = var.reset(L"html_ul_style", L"");
	html_link_class = var.reset(L"html_link_class", L"");
	html_link_style = var.reset(L"html_link_style", L"");
	html_section_class = var.reset(L"html_section_class", L"");
	html_section_style = var.reset(L"html_section_style", L"");
	html_hr_class = var.reset(L"html_hr_class", L"");
	html_hr_style = var.reset(L"html_hr_style", L"");
	html_img_class = var.reset(L"html_img_class", L"");
	html_img_style = var.reset(L"html_img_style", L"");

	continue_line = false;
	current_inline_tag = NULL;
	inline_tag_being_rednered = NULL;
	document_opened = false;
	current_var = UNKNOWN_VAR;
}

HtmlGenerator::~HtmlGenerator() {
	map<wstring, HtmlGenerator::AbstractInlineTag*>::iterator i;
	for (i = inline_tags.begin(); i != inline_tags.end(); ++i) {
		delete (*i).second;
	}
}

void HtmlGenerator::TagRenderer::write_attributes(ostream& out,
		const char* attr_names[], const char* attr_values[], size_t attr_count,
		bool exclude_class, bool exclude_style) {

	if (attr_count > 0) {
		out << " ";
	}

	for (size_t i = 0; i < attr_count; ++i) {
		if ((!exclude_class || char_traits<char>::compare(attr_names[i],
				"class", CLASS_STRLEN) != 0) && (!exclude_style || char_traits<
				char>::compare(attr_names[i], "style", STYLE_STRLEN) != 0)) {

			out << attr_names[i] << "='" << attr_values[i] << "' ";
		}
	}
}

void HtmlGenerator::TagRenderer::write_open(HtmlGenerator* generator,
		const char* tag_name, const char* attr_names[],
		const char* attr_values[], size_t attr_count, bool end, bool close) {

	*(generator->out) << "<" << tag_name << " ";

	bool exclude_class = false;
	bool exclude_style = false;

	var_id_t v = class_parameter(generator);
	if (v != UNKNOWN_VAR && !generator->var[v].as_string().empty()) {

		*(generator->out) << "class='";
		generator->var[v].markup.write(*(generator->out));
		for (size_t i = 0; i < attr_count; ++i) {
			if (char_traits<char>::compare(attr_names[i], "class", CLASS_STRLEN)
					== 0) {
				*(generator->out) << attr_values[i];
				exclude_class = true;
				break;
			}
		}
		*(generator->out) << "' ";
	}

	v = style_parameter(generator);
	if (v != UNKNOWN_VAR && !generator->var[v].as_string().empty()) {

		*(generator->out) << "style='";
		generator->var[v].markup.write(*(generator->out));
		for (size_t i = 0; i < attr_count; ++i) {
			if (char_traits<char>::compare(attr_names[i], "style", STYLE_STRLEN)
					== 0) {
				*(generator->out) << attr_values[i];
				exclude_style = true;
				break;
			}
		}
		*(generator->out) << "' ";
	}

	write_attributes(*(generator->out), attr_names, attr_values, attr_count,
			exclude_class, exclude_style);

	if (end) {
		if (close) {
			*(generator->out) << "/";
		}
		*(generator->out) << ">";
	}
}

void HtmlGenerator::TagRenderer::write_close(ostream& out, const char* tag_name) {
	out << "</" << tag_name << ">";
}

void HtmlGenerator::TagRenderer::open(HtmlGenerator* generator,
		const char* attr_names[], const char* attr_values[], size_t attr_count,
		bool end, bool close) {

	write_open(generator, tag_name(), attr_names, attr_values, attr_count, end,
			close);
}

void HtmlGenerator::TagRenderer::line(HtmlGenerator* generator) {
	if (generator->place_line_break) {
		*(generator->out) << "<br/>";
	}
	generator->decorate_text();
	generator->markup.write(*(generator->out));
}

void HtmlGenerator::TagRenderer::close(HtmlGenerator* generator) {
	write_close(*(generator->out), tag_name());
}

const char* HtmlGenerator::HeaderRenderer::tag_name() {
	static const char* names[] = { "h1", "h2", "h3", "h4", "h5", "h6" };
	return names[level - 1];
}

var_id_t HtmlGenerator::HeaderRenderer::class_parameter(HtmlGenerator* generator) {
	var_id_t vars[] = { generator->html_h1_class, generator->html_h2_class,
			generator->html_h3_class, generator->html_h4_class, generator->html_h5_class, generator->html_h6_class };
	return vars[level - 1];
}

var_id_t HtmlGenerator::HeaderRenderer::style_parameter(HtmlGenerator* generator) {
	var_id_t vars[] = { generator->html_h1_style, generator->html_h2_style,
			generator->html_h3_style, generator->html_h4_style, generator->html_h5_style, generator->html_h6_style };
	return vars[level - 1];
}

void HtmlGenerator::CiteRenderer::line(HtmlGenerator* generator) {

}

void HtmlGenerator::PreformatedRenderer::line(HtmlGenerator* generator) {
	if (generator->place_line_break) {
		*(generator->out) << endl;
	}
	generator->markup.write(*(generator->out));
}

void HtmlGenerator::OrderedListRenderer::line(HtmlGenerator* generator) {
	if (!generator->markup.empty()) {
		*(generator->out) << "<li>";
		generator->markup.write(*(generator->out));
		*(generator->out) << "</li>";
	}
}

void HtmlGenerator::UnorderedListRenderer::line(HtmlGenerator* generator) {
	if (!generator->markup.empty()) {
		*(generator->out) << "<li>";
		generator->markup.write(*(generator->out));
		*(generator->out) << "</li>";
	}
}

void HtmlGenerator::SectionRenderer::line(HtmlGenerator* generator) {

}

void HtmlGenerator::ImageRenderer::line(HtmlGenerator* generator) {
	switch (generator->image_tag_line) {
	case IMAGE_TAG_LINE_URL:
		*(generator->out) << "src='";
		generator->markup.write(*(generator->out));
		*(generator->out) << "' ";
		generator->image_tag_line = IMAGE_TAG_LINE_ALT;
		break;
	case IMAGE_TAG_LINE_ALT:
		*(generator->out) << "alt='";
		generator->markup.write(*(generator->out));
		*(generator->out) << "' ";
		generator->image_tag_line = IMAGE_TAG_LINE_IGNORE;
		break;
	case IMAGE_TAG_LINE_IGNORE:
		//Do nothing.
		break;
	}
}

void HtmlGenerator::ImageRenderer::close(HtmlGenerator* generator) {
	*(generator->out) << "/>";
}

void HtmlGenerator::AbstractInlineTag::append_markup_to_value(
		const MarkupBuilder& markup) {
	markup.append(value);
}

void HtmlGenerator::LinkInlineTag::open(HtmlGenerator* generator) {
	buffer.clear();
	buffer += "<a href='";
	buffer += value;
	buffer += "' ";

	wstring link_class = generator->var[generator->html_link_class].as_string();
	if (!link_class.empty()) {
		buffer += "class='";
		append_wchar_to_utf8(buffer, link_class.c_str());
		buffer += "' ";
	}

	wstring link_style = generator->var[generator->html_link_style].as_string();
	if (!link_style.empty()) {
		buffer += "style='";
		append_wchar_to_utf8(buffer, link_style.c_str());
		buffer += "' ";
	}

	buffer += ">";
	generator->markup.next_char().prepend(buffer.c_str());
}

void HtmlGenerator::LinkInlineTag::close(HtmlGenerator* generator) {
	generator->markup.last_char().append("</a>");
}

void HtmlGenerator::document() {
	document_opened = true;
	tag_stack.push(TAG_RENDERER_DOCUMENT);
	place_line_break = false;
}

void HtmlGenerator::header(int level) {
	TagRenderers renderer = (TagRenderers) ((int) TAG_RENDERER_HEADER1
			+ ((level < 0) ? 0 : level - 1));
	renderers[renderer]->open(this, NULL, NULL, 0, true, false);
	tag_stack.push(renderer);
	place_line_break = false;
}

const char* HtmlGenerator::alignment_css(Alignment alignment) {
	switch (alignment) {
	case ALIGN_LEFT:
		return "text-align:left;";
	case ALIGN_CENTER:
		return "text-align:center;";
	case ALIGN_RIGHT:
		return "text-align:right;";
	case ALIGN_JUSTIFY:
		return "text-align:justify;";
	default:
		throw StmlException(StmlException::UNKNOWN_ALIGNMENT);
	}
}

void HtmlGenerator::open_aligned_tag(TagRenderers renderer, Alignment alignment) {
	Alignment effective_alignment =
			(alignment != ALIGN_DEFAULT) ? alignment : parse_alignment(
					var[html_default_p_alignment].as_string());

	const char* attr_values[1];
	attr_values[0] = alignment_css(effective_alignment);

	const char* attr_names[] = { "style" };

	renderers[renderer]->open(this, attr_names, attr_values, 1, true, false);
}

void HtmlGenerator::paragraph(Alignment alignment) {
	if (tag_stack.empty() || tag_stack.top() != TAG_RENDERER_CITE) {
		open_aligned_tag(TAG_RENDERER_PARAGRAPH, alignment);
		tag_stack.push(TAG_RENDERER_PARAGRAPH);
	} else {
		open_aligned_tag(TAG_RENDERER_CITE_PARAGRAPH, alignment);
		tag_stack.push(TAG_RENDERER_CITE_PARAGRAPH);
	}
	place_line_break = false;
}

void HtmlGenerator::link(const wstring& name) {
	map<wstring, HtmlGenerator::AbstractInlineTag*>::iterator tag =
			inline_tags.find(name);
	if (tag != inline_tags.end()) {
		throw StmlException(StmlException::INLINE_TAG_ALREADY_EXISTS);
	}

	current_inline_tag = new HtmlGenerator::LinkInlineTag(name);
	inline_tags[name] = current_inline_tag;

	tag_stack.push(TAG_RENDERER_LINK);
	place_line_break = false;
}

void HtmlGenerator::cite(Alignment alignment) {
	renderers[TAG_RENDERER_CITE]->open(this, NULL, NULL, 0, true, false);
	tag_stack.push(TAG_RENDERER_CITE);
	place_line_break = false;
}

void HtmlGenerator::verse() {
	renderers[TAG_RENDERER_VERSE]->open(this, NULL, NULL, 0, true, false);
	tag_stack.push(TAG_RENDERER_VERSE);
	place_line_break = false;
}

void HtmlGenerator::preformated() {
	renderers[TAG_RENDERER_PREFORMATED]->open(this, NULL, NULL, 0, true, false);
	tag_stack.push(TAG_RENDERER_PREFORMATED);
	place_line_break = false;
}

void HtmlGenerator::line_break() {
	renderers[TAG_RENDERER_LINE_BREAK]->open(this, NULL, NULL, 0, true, true);
	place_line_break = false;
}

void HtmlGenerator::ordered_list() {
	renderers[TAG_RENDERER_ORDERED_LIST]->open(this, NULL, NULL, 0, true, false);
	tag_stack.push(TAG_RENDERER_ORDERED_LIST);
	place_line_break = false;
}

void HtmlGenerator::unordered_list() {
	renderers[TAG_RENDERER_UNORDERED_LIST]->open(this, NULL, NULL, 0, true,
			false);
	tag_stack.push(TAG_RENDERER_UNORDERED_LIST);
	place_line_break = false;
}

void HtmlGenerator::comment() {
	tag_stack.push(TAG_RENDERER_COMMENT);
	place_line_break = false;
}

void HtmlGenerator::section() {
	renderers[TAG_RENDERER_SECTION]->open(this, NULL, NULL, 0, true, false);
	tag_stack.push(TAG_RENDERER_SECTION);
	place_line_break = false;
}

void HtmlGenerator::horizontal_line() {
	renderers[TAG_RENDERER_HORIZONTAL_LINE]->open(this, NULL, NULL, 0, true,
			true);
	place_line_break = false;
}

void HtmlGenerator::parameter(const wstring& name) {
	current_var = UNKNOWN_VAR;
	if (!name.empty()) {
		current_var = var.reset(name.c_str(), L"");
	}
	tag_stack.push(TAG_RENDERER_PARAMETER);
	place_line_break = false;
}

void HtmlGenerator::image(int width, int height, bool width_percent,
		bool height_percent, Alignment alignment) {

	image_tag_line = IMAGE_TAG_LINE_URL;

	image_style.clear();

	char int_str[MAX_INT_SIZE];
	if (width > 0) {
		sprintf(int_str, "%d", width);
		image_style.append("width:").append(int_str).append(
				(width_percent) ? "%" : "px") .append(";");
	}

	if (height > 0) {
		sprintf(int_str, "%d", height);
		image_style.append("height:").append(int_str).append(
				(height_percent) ? "%" : "px") .append(";");
	}

	if (alignment == ALIGN_LEFT) {
		image_style.append("float:left;");
	} else if (alignment == ALIGN_RIGHT) {
		image_style.append("float:right;");
	}

	if (!image_style.empty()) {
		const char* attr_names[] = { "style" };
		const char* attr_values[1];

		attr_values[0] = image_style.c_str();

		renderers[TAG_RENDERER_IMAGE]->open(this, attr_names, attr_values, 1,
				false, false);
	} else {
		renderers[TAG_RENDERER_IMAGE]->open(this, NULL, NULL, 0, false, false);
	}

	tag_stack.push(TAG_RENDERER_IMAGE);
	place_line_break = false;
}

void HtmlGenerator::generate_doc_header() {
	*out << "<html><head>";
	*out
			<< "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">";
	if (!var[html_doc_title].as_string().empty()) {
		*out << "<title>";
		var[html_doc_title].markup.write(*out);
		*out << "</title>";
	}
	if (!var[html_embedded_css].as_string().empty()) {
		*out << "<style type='text/css'>";
		var[html_embedded_css].markup.write(*out);
		*out << "</style>";
	}
	*out << "</head>";

	*out << "<body ";
	if (!var[html_body_class].as_string().empty()) {
		*out << "class='";
		var[html_body_class].markup.write(*out);
		*out << "' ";
	}
	if (!var[html_body_style].as_string().empty()) {
		*out << "style='";
		var[html_body_style].markup.write(*out);
		*out << "' ";
	}
	*out << ">";
}

void HtmlGenerator::close_tag() {
	if (tag_stack.empty()) {
		throw StmlException(StmlException::UNEXPECTED_CLOSE_TAG);
	}

	if (!markup.empty()) {
		line_end();
	}

	TagRenderers top = tag_stack.top();

	if (top == TAG_RENDERER_DOCUMENT) {
		generate_doc_header();
	}

	//If there is a renderer for the tag currently on top,
	//render closing tag.
	if (renderers[top].get()) {
		renderers[top]->close(this);

		if (!var[html_no_line_breaks].as_boolean()) {
			*out << endl;
		}
	}

	tag_stack.pop();

	current_inline_tag = NULL;
	current_var = UNKNOWN_VAR;
}

void HtmlGenerator::inject_variable(const wstring& variable_name) {
	try {
		markup << var[variable_name].markup;
	}
	catch(const out_of_range&) {
		throw StmlException(StmlException::VARIABLE_NOT_DECLARED);
	}
}

void HtmlGenerator::open_inline_tag(const wstring& tag_name) {
	map<wstring, HtmlGenerator::AbstractInlineTag*>::iterator tag =
			inline_tags.find(tag_name);

	if (tag != inline_tags.end()) {
		inline_tag_being_rednered = (*tag).second;
		inline_tag_being_rednered->open(this);
	}
}

void HtmlGenerator::close_inline_tag() {
	if (inline_tag_being_rednered) {

		inline_tag_being_rednered->close(this);
		inline_tag_being_rednered = NULL;
	}
}

void HtmlGenerator::text_char(wchar_t c) {
	if (!tag_stack.empty() && (tag_stack.top() == TAG_RENDERER_COMMENT
			|| tag_stack.top() == TAG_RENDERER_DOCUMENT)) {
		return;
	}

	markup << c;

	if (c == L'&') {
		markup.substitute(markup.last_char_index(), 1, "&amp;");
	} else if (c == L'<') {
		markup.substitute(markup.last_char_index(), 1, "&lt;");
	} else if (c == L'>') {
		markup.substitute(markup.last_char_index(), 1, "&gt;");
	}
}

void HtmlGenerator::open_bold() {
	markup.next_char().prepend("<b>");
}

void HtmlGenerator::close_bold() {
	markup.last_char().append("</b>");
}

void HtmlGenerator::open_italic() {
	markup.next_char().prepend("<i>");
}

void HtmlGenerator::close_italic() {
	markup.last_char().append("</i>");
}

void HtmlGenerator::stress_mark() {
	markup.last_char().append(HTML_STRESS_MARK);
}

void HtmlGenerator::line_continue() {
	continue_line = true;
}

void HtmlGenerator::decorate_text() {
	wstring text = markup.get_text();

	language->process_punctuation(markup, HTML_DASH, quotes);

	TagRenderers top_tag = (tag_stack.empty()) ? TAG_RENDERER_PARAGRAPH
			: tag_stack.top();

	if (!var[html_no_shys].as_boolean() && top_tag
			!= TAG_RENDERER_PREFORMATED && !(top_tag >= TAG_RENDERER_HEADER1
			&& top_tag <= TAG_RENDERER_HEADER6)) {

		tokenizer->reset();
		while (tokenizer->next_token(text)) {
			Tokenizer::Token current_token = tokenizer->get_current_token();

			if (current_token.type == WORD_TOKEN) {
				language->hyphenate(markup, current_token.start,
						current_token.length, HTML_SHY);
			}
		}
	}
}

void HtmlGenerator::line_end() {

	if (tag_stack.empty() || tag_stack.top() == TAG_RENDERER_SECTION
			|| tag_stack.top() == TAG_RENDERER_CITE) {

		//If we are in root, section or cite,
		//render default paragraph or, if $no_default_paragraphs specified, plain text.

		if (var[html_no_default_paragraphs].as_boolean()) {
			markup.write(*out);
			*out << endl;
			markup.clear();
		} else if (!markup.empty()) {
			paragraph(ALIGN_DEFAULT);
			close_tag();
		}
	} else {
		if (current_inline_tag) {
			current_inline_tag->append_markup_to_value(markup);
		} else if (current_var != UNKNOWN_VAR) {
			var[current_var].markup << markup;
		} else {
			TagRenderers top = tag_stack.top();
			//If there is a renderer for the tag currently on top,
			//render the line within tag.
			if (renderers[top].get()) {
				renderers[top]->line(this);
			}

			//If it is not specified explicitly that the line must be continued,
			//the next not empty line will start with line break.
			if (!continue_line && !markup.empty()) {
				place_line_break = true;
			} else {
				place_line_break = false;
			}
			continue_line = false;
		}

		markup.clear();
	}
}

void HtmlGenerator::close_document() {
	if (document_opened) {
		*out << "</body></html>";
	}
}
