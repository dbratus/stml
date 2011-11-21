#include "../../../include/markup_builder.hpp"
#include "../../../include/languages/russian/russian_tokenizer.hpp"
#include "../../../include/languages/russian/russian_language.hpp"

using namespace std;
using namespace stml;

bool is_prefix(const wstring& str) {
    return
        str == L"не"
        || str == L"со"
        || str == L"пре"
        || str == L"по"
        || str == L"при"
        || str == L"пере"
        || str == L"передо"
        || str == L"противо"
        || str == L"свето"
        || str == L"одно"
        || str == L"дву"
        || str == L"два"
        || str == L"три"
        || str == L"четверо"
        || str == L"пяти"
        || str == L"шести"
        || str == L"семи"
        || str == L"восьми"
        || str == L"девяти"
        || str == L"десяти"
        || str == L"двадцати"
        || str == L"тридцати"
        || str == L"сорока"
        || str == L"пятидесяти"
        || str == L"шестидесяти"
        || str == L"семидесяти"
        || str == L"восьмидесяти"
        || str == L"девяносто"
        || str == L"сто"
        ;
}

bool RussianLanguage::is_shy_1_allowed(const wstring& text, size_t start, size_t index, size_t limit) {
    if (index == start || index + 2 >= limit) {
        return false;
    }

    return
        is_vowel(text[index + 1]) ||
        is_vowel(text[index + 2]) ||
        (is_soft_modifier(text[index + 2]) && index + 3 < limit && is_vowel(text[index + 3])) ||
        is_prefix(text.substr(start, index - start + 1));
}

bool RussianLanguage::is_shy_2_allowed(const wstring& text, size_t index, size_t limit) {
    return
        index + 3 < limit &&
        !(is_sibilant(text[index + 1]) && !is_vowel(text[index + 2]) && !is_vowel(text[index + 3])) &&
        !is_modifier(text[index + 2]);
}

bool RussianLanguage::is_shy_3_allowed(const wstring& text, size_t index, size_t limit) {
    return
        index + 4 < limit &&
        !is_modifier(text[index + 3]) &&
        (is_vowel(text[index + 3]) || is_vowel(text[index + 4]));
}

bool RussianLanguage::is_acronym(const wstring& text, size_t start, size_t length) {
    int upper_case_count = 0;
    for(size_t i = 0;i < length; ++i) {
        if (is_upper(text[start + i])) {
            ++upper_case_count;
            if (upper_case_count > 1) {
                return true;
            }
        }
    }

    return false;
}

wchar_t RussianLanguage::to_lower(wchar_t c) const {
    if (is_upper(c)) {
        return
            char_traits<wchar_t>::to_char_type(
                char_traits<wchar_t>::to_int_type(c) + UPPER_LOWER_RANGE
            );
    }
    else {
        return c;
    }
}

wchar_t RussianLanguage::to_upper(wchar_t c) const {
    if (is_lower(c)) {
        return
            char_traits<wchar_t>::to_char_type(
                char_traits<wchar_t>::to_int_type(c) - UPPER_LOWER_RANGE
            );
    }
    else {
        return c;
    }
}

bool RussianLanguage::is_word_char(wchar_t c) const {
    return RussianLanguage::is_cyrillic(c) || EuropeanLanguage::is_latin(c);
}

void RussianLanguage::hyphenate(MarkupBuilder& builder, size_t start, size_t length, const char* shy) const {
    const wstring text = builder.get_text();

    if (is_acronym(text, start, length)) {
        return;
    }

    size_t limit = start + length;

    for(size_t i = 0; i < length; ++i) {
        size_t at = start + i;
        if (is_vowel(text[at])) {
            if (is_shy_1_allowed(text, start, at, limit)) {
                builder[at].append(shy);
            }
            else if (is_shy_2_allowed(text, at, limit)) {
                builder[at + 1].append(shy);
            }
            else if (is_shy_3_allowed(text, at, limit)) {
                builder[at + 2].append(shy);
            }
        }
    }
}

Tokenizer* RussianLanguage::create_tokenizer() const {
    return new RussianTokenizer(this);
}
