#ifndef RUSSIAN_LANGUAGE_HPP_
#define RUSSIAN_LANGUAGE_HPP_

#include "../european_language.hpp"

namespace stml {

class RussianLanguage : public EuropeanLanguage {
    static const int UPPER_LOWER_RANGE = 32;

    static bool is_shy_1_allowed(const std::wstring& text, size_t start, size_t index, size_t limit);
    static bool is_shy_2_allowed(const std::wstring& text, size_t index, size_t limit);
    static bool is_shy_3_allowed(const std::wstring& text, size_t index, size_t limit);
    static bool is_acronym(const std::wstring& word, size_t start, size_t length);

protected:
    QuoteTypes opened_quote() const { return QUOTE_FRENCH_OPENED; }
    QuoteTypes closed_quote() const { return QUOTE_FRENCH_CLOSED; }
    QuoteTypes opened_alt_quote() const { return QUOTE_GERMAN_OPENED; }
    QuoteTypes closed_alt_quote() const { return QUOTE_GERMAN_CLOSED; }

public:
    bool is_word_char(wchar_t c) const;
    wchar_t to_lower(wchar_t c) const;
    wchar_t to_upper(wchar_t c) const;

    void hyphenate(MarkupBuilder& builder, size_t start, size_t length, const char* shy) const;
    Tokenizer* create_tokenizer() const;

    static inline bool is_cyrillic(wchar_t c) {
        return (c >= L'А' && c <= L'Я') || (c >= L'а' && c <= L'я') || c == L'Ё' || c == L'ё';
    }

    static inline bool is_upper(wchar_t c) {
        return
            (c >= L'A' && c <= L'Z') ||
            ((c >= L'А' && c <= L'Я') || c == L'Ё');
    }

    static inline bool is_lower(wchar_t c) {
        return
            (c >= L'a' && c <= L'z') ||
            ((c >= L'а' && c <= L'я') || c == L'ё');
    }

    static inline bool is_vowel(wchar_t c) {
        return
            std::char_traits<wchar_t>::find(L"ЁУЕЫАОЭЯИЮ", 10, c) ||
            std::char_traits<wchar_t>::find(L"ёуеыаоэяию", 10, c);
    }

    static inline bool is_soft_modifier(wchar_t c) {
        return c == L'ь' || c == L'Ь';
    }

    static inline bool is_hard_modifier(wchar_t c) {
        return c == L'ъ' || c == L'Ъ';
    }

    static inline bool is_modifier(wchar_t c) {
        return is_soft_modifier(c) || is_hard_modifier(c);
    }

    static inline bool is_sibilant(wchar_t c) {
        return
            std::char_traits<wchar_t>::find(L"СЗ", 2, c) ||
            std::char_traits<wchar_t>::find(L"сз", 2, c);
    }
};

}

#endif /* RUSSIAN_LANGUAGE_HPP_ */
