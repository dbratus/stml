#ifndef RUSSIAN_TOKENIZER_HPP_
#define RUSSIAN_TOKENIZER_HPP_

#include "../european_tokenizer.hpp"

namespace stml {

class RussianTokenizer : public EuropeanTokenizer {
protected:
    bool is_word_char(wchar_t c);

public:
    RussianTokenizer(const Language* language);

};

}

#endif /* RUSSIAN_TOKENIZER_HPP_ */
