#ifndef EUROPEAN_LANGUAGE_HPP_
#define EUROPEAN_LANGUAGE_HPP_

#include "language.hpp"

namespace stml {

class EuropeanLanguage : public Language {
protected:
    virtual QuoteTypes opened_quote() const = 0;
    virtual QuoteTypes closed_quote() const = 0;
    virtual QuoteTypes opened_alt_quote() const = 0;
    virtual QuoteTypes closed_alt_quote() const = 0;

public:
    static inline bool is_latin(wchar_t c) {
        return (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z');
    }

    void process_punctuation(
        MarkupBuilder& builder,
        const char* dash,
        const std::map<int,std::string>& quotes) const;
};

}

#endif /* EUROPEAN_LANGUAGE_HPP_ */
