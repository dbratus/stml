#include "../../../include/languages/russian/russian_language.hpp"
#include "../../../include/languages/russian/russian_tokenizer.hpp"

using namespace stml;

RussianTokenizer::RussianTokenizer(const Language* language) : EuropeanTokenizer(language) {
}

bool RussianTokenizer::is_word_char(wchar_t c) {
    return RussianLanguage::is_cyrillic(c) || EuropeanLanguage::is_latin(c);
}
