#include "../../include/languages/tokenizer.hpp"

using namespace stml;

Tokenizer::Tokenizer(const Language* language) {
    this->language = language;
    reset();
}

const Tokenizer::Token& Tokenizer::get_current_token() {
    return current_token;
}

void Tokenizer::reset() {
    current_token.start = 0;
    current_token.length = 0;
}
