#ifndef ABSTRACT_GENERATOR_H_
#define ABSTRACT_GENERATOR_H_

#include <iostream>
#include <string>

namespace stml {

struct ImageSize {
	int width;
	int height;
	Units width_unit;
	Units height_unit;
};

class AbstractGenerator {
protected:

	class AttributesWriter {
	public:
		virtual void write_attributes(std::ostream& out) const = 0;
	};

    std::ostream* out;

public:
    virtual ~AbstractGenerator();

    void set_output(std::ostream* out);
    std::ostream* get_output() const;

    /**
     * <doc> tag.
     */
    virtual void document() = 0;

    /**
     * <h> tag.
     *
     * @param	level	Level of the header.
     */
    virtual void header(int level) = 0;

    /**
     * <p> tag or default paragraph.
     *
     * @param	alignment	Alignment of the text within the paragraph.
     *
     */
    virtual void paragraph(Alignments alignment) = 0;

    /**
     * <link> tag.
     *
     * @param	name	Name of the link.
     *
     */
    virtual void link(const std::wstring& name) = 0;

    /**
     * <c> tag.
     *
     * @param	alignment	Alignment of the text within the cite.
     *
     */
    virtual void cite(Alignments alignment) = 0;

    /**
     * <verse> tag.
     */
    virtual void verse() = 0;

    /**
     * <pre> tag.
     */
    virtual void preformated() = 0;

    /**
     * <br> tag.
     */
    virtual void line_break() = 0;

    /**
     * <ol> tag.
     */
    virtual void ordered_list() = 0;

    /**
     * <ul> tag.
     */
    virtual void unordered_list() = 0;

    /**
     * <!> tag.
     */
    virtual void comment() = 0;

    /**
     * <s> tag.
     */
    virtual void section() = 0;

    /**
     * <hr> tag.
     */
    virtual void horizontal_line() = 0;

    /**
     * <$...> tag.
     *
     * @param	name	Name of the variable.
     */
    virtual void variable(const std::wstring& name) = 0;

    /**
     * <#> tags.
     *
     * @param	level	Level of the item.
     *
     */
    virtual void ordered_list_item(int level) = 0;

    /**
     * <*> tags.
     *
     * @param	level	Level of the item.
     *
     */
    virtual void unordered_list_item(int level) = 0;

    /**
     * <img> tag.
     *
     * @param	size		Size of the image.
     * @param	alignment	Alignment of the image. ALIGN_DEFAULT if not specified.
     *
     */
    virtual void image(ImageSize* size, Alignments alignment) = 0;

    /**
     * <.> tag.
     */
    virtual void terminator() = 0;

    virtual void close_tag() = 0;
    virtual void inject_variable(const std::wstring& variable_name) = 0;
    virtual void open_inline_tag(const std::wstring& tag_name) = 0;
    virtual void close_inline_tag() = 0;
    virtual void text_char(wchar_t c) = 0;
    virtual void open_bold() = 0;
    virtual void close_bold() = 0;
    virtual void open_italic() = 0;
    virtual void close_italic() = 0;
    virtual void stress_mark() = 0;
    virtual void line_continue() = 0;
    virtual void line_end() = 0;
    virtual void close_document() = 0;
};

/**
 * Creates a generator instance by given generator type.
 *
 * @param 	generator_type	Type of the generator to create.
 *
 * @return Pointer to the new generator.
 */
AbstractGenerator* create_generator(GeneratorTypes generator_type);

}

#endif /* ABSTRACT_GENERATOR_H_ */
