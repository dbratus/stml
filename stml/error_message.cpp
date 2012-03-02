#include <stml_exception.hpp>

using namespace stml;

const char* get_error_message(StmlException::Codes code) {
    switch (code) {
    case StmlException::UNSUPPORTED_GENERATOR_TYPE:
        return "Unsupported type of generator";
    case StmlException::INTERNAL_TO_OUTPUT_CONVERSION_NOT_SUPPORTED:
        return "Internal to output conversion is not supported";
    case StmlException::INPUT_TO_INTERNAL_CONVERSION_NOT_SUPPORTED:
        return "Input to internal conversion is not supported";
    case StmlException::CHARACTER_CANNOT_BE_CONVERTED_TO_OUTPUT_FORMAT:
        return "Character cannot be converted to output format";
    case StmlException::UNKNOWN_TAG:
        return "Unknown tag";
    case StmlException::NAMELESS_INLINE_TAG:
        return "Nameless inline tag";
    case StmlException::UNKNOWN_ALIGNMENT:
        return "Unknown alignment";
    case StmlException::UNEXPECTED_CLOSE_TAG:
        return "Unexpected close tag";
    case StmlException::INLINE_TAG_ALREADY_EXISTS:
        return "Inline tag already exists";
    case StmlException::UNSUPPORTED_HEADER_LEVEL:
        return "Unsupported header level";
    case StmlException::VARIABLE_NOT_DECLARED:
    	return "Variable is not declared";
    default:
        return "";
    }
}
