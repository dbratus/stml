#ifndef TOKENIZER_HPP_
#define TOKENIZER_HPP_

#include "../stml.hpp"
#include "language.hpp"

#include <string>

namespace stml {

enum LanguageTokens {
    WORD_TOKEN = 1,
    SINGLE_CHAR_TOKEN,
    WHITESPACE_TOKEN,
    LINE_BREAK_TOKEN
};

class Tokenizer {
public:
    struct Token {
        size_t start;
        size_t length;
        LanguageTokens type;
    };

protected:
    const Language* language;
    Token current_token;

    Tokenizer(const Language* language);

public:
    virtual bool next_token(const std::wstring& text) = 0;
    const Token& get_current_token();
    void reset();
};

}

#endif /* TOKENIZER_HPP_ */
