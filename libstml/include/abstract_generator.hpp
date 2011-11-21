#ifndef ABSTRACT_GENERATOR_H_
#define ABSTRACT_GENERATOR_H_

#include <iostream>
#include <string>

#define PARAM_FALSE_VALUE ("n")

namespace stml {

class AbstractGenerator {
protected:
    class Parameter {
    public:

        std::wstring name;
        std::string value;

        void append_markup_to_value(const MarkupBuilder& markup);

        const std::string& as_string() const;
        bool as_boolean() const;
    };

    std::ostream* out;

public:
    virtual ~AbstractGenerator();

    void set_output(std::ostream* out);

    virtual void document() = 0;
    virtual void header(int level) = 0;
    virtual void paragraph(Alignment alignment) = 0;
    virtual void link(const std::wstring& name) = 0;
    virtual void cite(Alignment alignment) = 0;
    virtual void verse() = 0;
    virtual void preformated() = 0;
    virtual void line_break() = 0;
    virtual void ordered_list() = 0;
    virtual void unordered_list() = 0;
    virtual void comment() = 0;
    virtual void section() = 0;
    virtual void horizontal_line() = 0;
    virtual void parameter(const std::wstring& name) = 0;

    /**
     * <img> tag.
     *
     * @param	width			Width of the image in pexels or percent.
     * 							Zero if '?' is specified.
     * @param	height			Height of the image in pixels or percent.
     * 							Zero if '?' is specified.
     * @param	width_percent	Indicates whether width is specified in percent.
     * @param	height_percent	Indicates whether height is specified in percent.
     * @param	alignment		Alignment of the image. ALIGN_DEFAULT if not specified.
     *
     */
    virtual void image(int width, int height, bool width_percent, bool height_percent, Alignment alignment) = 0;

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