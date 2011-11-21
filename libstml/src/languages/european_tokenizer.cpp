#include "../../include/languages/european_tokenizer.hpp"

using namespace stml;
using namespace std;

EuropeanTokenizer::EuropeanTokenizer(const Language* language) : Tokenizer(language) {
}

bool EuropeanTokenizer::next_token(const wstring& text) {
    current_token.start += current_token.length;
    size_t text_length = text.length();

    if (current_token.start == text_length) {
        current_token.length = 0;
        return false;
    }

    current_token.length = 1;

    wchar_t c = text[current_token.start];
    if (c == L' ' || c == L'\t') {
        current_token.type = WHITESPACE_TOKEN;
    }
    else if (c == '\n') {
        current_token.type = LINE_BREAK_TOKEN;
    }
    else if (language->is_word_char(c)) {
        size_t i = current_token.start + 1;
        while(i < text_length && language->is_word_char(text[i])) {
            ++i;
        }
        current_token.length = i - current_token.start;
        current_token.type = WORD_TOKEN;
    }
    else {
        current_token.type = SINGLE_CHAR_TOKEN;
    }

    return true;
}
