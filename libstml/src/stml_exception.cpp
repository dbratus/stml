#include "../include/stml_exception.hpp"

using namespace stml;

StmlException::StmlException(Codes code) {
    this->code = code;
    this->line_no = 0;
}

StmlException::Codes StmlException::get_code() const {
    return code;
}

void StmlException::set_line_no(unsigned int value) {
    line_no = value;
}

unsigned int StmlException::get_line_no() const {
    return line_no;
}
