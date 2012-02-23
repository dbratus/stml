#ifndef STML_EXCEPTION_H_
#define STML_EXCEPTION_H_

#include <string>

namespace stml {

/**
 * Represents all failures within stml namespace.
 */
class StmlException {
public:
    enum Codes {
        UNSUPPORTED_GENERATOR_TYPE = 1,
        INTERNAL_TO_OUTPUT_CONVERSION_NOT_SUPPORTED,
        INPUT_TO_INTERNAL_CONVERSION_NOT_SUPPORTED,
        CHARACTER_CANNOT_BE_CONVERTED_TO_INTERNAL_FORMAT,
        CHARACTER_CANNOT_BE_CONVERTED_TO_OUTPUT_FORMAT,
        UNKNOWN_TAG,
        NAMELESS_INLINE_TAG,
        UNKNOWN_ALIGNMENT,
        UNEXPECTED_CLOSE_TAG,
        INLINE_TAG_ALREADY_EXISTS,
        UNSUPPORTED_HEADER_LEVEL,
        VARIABLE_NOT_DECLARED,
        LIST_INDEX_OVERFLOW,
        MAX_HEADER_DEPTH_EXCEEDED,
        MAX_LIST_DEPTH_EXCEEDED,
        LIST_LEVEL_HOP
    };

private:

    Codes code;
    unsigned int line_no;

public:
    StmlException(Codes code);

    /**
     * Returns the code of the exception.
     */
    Codes get_code() const;

    /**
     * Sets the line number (in STML) from which the exception has been thrown.
     */
    void set_line_no(unsigned int value);

    /**
     * Returns the line number (in STML) from which the exception has been thrown.
     */
    unsigned int get_line_no() const;
};

}

#endif /* STML_EXCEPTION_H_ */
