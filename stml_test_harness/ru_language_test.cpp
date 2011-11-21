#include "ru_language_test.hpp"
#include "../libstml/include/markup_builder.hpp"
#include "../libstml/include/languages/tokenizer.hpp"
#include "../libstml/include/languages/russian/russian_language.hpp"
#include <memory>
#include <string>
#include <cassert>
#include <sstream>

using namespace std;
using namespace stml;

void ru_language_test() {
    RussianLanguage lang;
    auto_ptr<Tokenizer> tokenizer(lang.create_tokenizer());

    wstring text = L"мама  мыла раму.\n";
    LanguageTokens tokens_expected[] = {
        WORD_TOKEN,
        WHITESPACE_TOKEN,
        WHITESPACE_TOKEN,
        WORD_TOKEN,
        WHITESPACE_TOKEN,
        WORD_TOKEN,
        SINGLE_CHAR_TOKEN,
        LINE_BREAK_TOKEN
    };

    int i = 0;
    while (tokenizer->next_token(text)) {
        assert(tokenizer->get_current_token().type == tokens_expected[i]);
        ++i;
    }
    assert(i == 8);

    tokenizer->reset();

    MarkupBuilder bld;
    bld << L"один предсказатель двадцать якобы иметь обязательно этот лестничной удалились дверью объективный необъяснимый";

    while (tokenizer->next_token(bld.get_text())) {
        Tokenizer::Token current_token = tokenizer->get_current_token();

        if (current_token.type == WORD_TOKEN) {
            lang.hyphenate(bld, current_token.start, current_token.length, "-");
        }
    }

    bld.write(cout);
}
