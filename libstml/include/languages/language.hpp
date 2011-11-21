#ifndef LANGUAGE_HPP_
#define LANGUAGE_HPP_

#include "../stml.hpp"

#include <map>

namespace stml {

class Language {
public:
    virtual bool is_word_char(wchar_t c) const = 0;
    virtual wchar_t to_lower(wchar_t c) const = 0;
    virtual wchar_t to_upper(wchar_t c) const = 0;

    virtual void hyphenate(MarkupBuilder& builder, size_t start, size_t length, const char* shy) const = 0;
    virtual void process_punctuation(
        MarkupBuilder& builder,
        const char* dash,
        const std::map<int,std::string>& quotes) const = 0;
    virtual Tokenizer* create_tokenizer() const = 0;
};

}

#endif /* LANGUAGE_HPP_ */
