#ifndef TEX_GENERATOR_H_
#define TEX_GENERATOR_H_

#define TEX_STRESS_MARK ("\\'")
#define TEX_DASH ("---")
#define TEX_SHY ("\\-")

#include "../../include/markup_builder.hpp"
#include "../../include/languages/language.hpp"
#include "../../include/languages/tokenizer.hpp"

#include <memory>
#include <map>
#include <stack>
#include <vector>

namespace stml {

class TexGenerator : public AbstractGenerator {

    enum TexRenderers {
        TEX_RENDERER_DOCUMENT,
        TEX_RENDERER_CHAPTER,
        TEX_RENDERER_SECTION,
        TEX_RENDERER_SUBSECTION,
        TEX_RENDERER_SUBSUBSECTION,
        TEX_RENDERER_PARAGRAPH,
        TEX_RENDERER_QUOTATION,
        TEX_RENDERER_VERSE,
        TEX_RENDERER_VERBATIM,
        TEX_RENDERER_ITEMIZE,
        TEX_RENDERER_ENUMERATE,
        TEX_RENDERER_RULE,
        TEX_RENDERER_STML_SECTION,
        TEX_RENDERER_COMMENT,
        TEX_RENDERER_PARAMETER,
        TEX_RENDERER_LINK,
        TEX_RENDERERS_COUNT
    };

    enum Parameters {
        PARAM_UNKNOWN,
        PARAM_CHAPTER_LINE_SKIP,
        PARAM_CHAPTER_SUBTITLE_FORMAT,
        PARAM_CHAPTER_NUMBERS,
        PARAM_SECTION_NUMBERS,
        PARAM_SUBSECTION_NUMBERS,
        PARAM_SUBSUBSECTION_NUMBERS,
        PARAM_BR_SIZE,
        PARAM_HR_HEIGHT,
        PARAM_HR_WIDTH,
        PARAMS_COUNT
    };

    class TexRenderer {
    public:
        virtual void line(TexGenerator* generator);
        virtual void end(TexGenerator* generator) = 0;
    };

    typedef std::auto_ptr<TexRenderer> TexRendererPtr;

    class ParagraphRenderer : public TexRenderer {
    public:
        void end(TexGenerator* generator);
    };

    class CommandRenderer : public TexRenderer {
        std::string command;

    public:
        CommandRenderer(const char* command) { this->command = command; }

        void begin(TexGenerator* generator, bool starred);
        void end(TexGenerator* generator);
    };

    class ChaperRenderer : public CommandRenderer {
    public:
        ChaperRenderer() : CommandRenderer("chapter") { }

        void line(TexGenerator* generator);
    };

    class EnvironmentRenderer : public TexRenderer {
        std::string environment;

    public:
        EnvironmentRenderer(const char* environment) { this->environment = environment; }

        void begin(TexGenerator* generator);
        void end(TexGenerator* generator);
    };

    std::map<int, std::string> quotes;

    MarkupBuilder markup;
    std::stack<TexRenderers, std::vector<TexRenderers> > tag_stack;
    std::auto_ptr<Language> language;
    std::auto_ptr<Tokenizer> tokenizer;
    Parameter parameters[PARAMS_COUNT];
    TexRendererPtr renderers[TEX_RENDERERS_COUNT];

    Parameters current_parameter;
    bool continue_line;
    bool place_line_break;

    Parameters get_parameter_by_name(const std::wstring& name) const;
    void decorate_text();

public:
    TexGenerator();
    ~TexGenerator();

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
    void stress_mark();
    void line_continue();
    void line_end();
    void close_document();
};

}


#endif /* TEX_GENERATOR_H_ */
