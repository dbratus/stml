#ifndef TEX_GENERATOR_H_
#define TEX_GENERATOR_H_

#define TEX_STRESS_MARK ("\\'")
#define TEX_DASH ("---")
#define TEX_SHY ("\\-")

#include "../../include/markup_builder.hpp"
#include "../../include/languages/language.hpp"
#include "../../include/languages/tokenizer.hpp"
#include "../../include/variables_manager.hpp"
#include "../../include/list_items_counter.hpp"

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
        TEX_RENDERER_VARIABLE,
        TEX_RENDERER_LINK,
        TEX_RENDERERS_COUNT
    };

    static const int MAX_ML_LIST_DEPTH = 4;

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

    class ListRenderer : public EnvironmentRenderer {
    public:
    	ListRenderer(const char* environment) : EnvironmentRenderer(environment) { }

    	void line(TexGenerator* generator);
    };

    std::map<int, std::string> quotes;

    MarkupBuilder markup;
    std::stack<TexRenderers, std::vector<TexRenderers> > tag_stack;
    std::auto_ptr<Language> language;
    std::auto_ptr<Tokenizer> tokenizer;
    TexRendererPtr renderers[TEX_RENDERERS_COUNT];
    ListItemsCounter list_items_counter;

    var_id_t tex_chapter_line_skip;
    var_id_t tex_chapter_subtitle_format;
    var_id_t tex_chapter_numbers;
    var_id_t tex_section_numbers;
    var_id_t tex_subsection_numbers;
    var_id_t tex_subsubsection_numbers;
    var_id_t tex_br_size;
    var_id_t tex_hr_width;
    var_id_t tex_hr_height;

    VariablesManager var;
    var_id_t current_var;
    bool continue_line;
    bool place_line_break;

    void decorate_text();
    void ml_list(TexRenderers renderer, int level);

public:
    TexGenerator();
    virtual ~TexGenerator();

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
    void variable(const std::wstring& name);
    void image(int width, int height, bool width_percent, bool height_percent, Alignment alignment);
	void ordered_list_item(int level);
	void unordered_list_item(int level);
	void terminator();

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
