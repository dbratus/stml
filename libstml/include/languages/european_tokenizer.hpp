#ifndef EUROPEAN_TOKENIZER_HPP_
#define EUROPEAN_TOKENIZER_HPP_

#include "tokenizer.hpp"

namespace stml {

class EuropeanTokenizer : public Tokenizer {
protected:
    virtual bool is_word_char(wchar_t c) = 0;

    EuropeanTokenizer(const Language* language);

public:
    bool next_token(const std::wstring& text);
};

}

#endif /* EUROPEAN_TOKENIZER_HPP_ */
