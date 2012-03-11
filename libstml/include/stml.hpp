#ifndef STML_H_
#define STML_H_

#include <iostream>
#include <memory>

namespace stml {

class Parser;
class StmlException;
class AbstractGenerator;
class HtmlGenerator;
class TexGenerator;
class InputReader;
class AbstractParserState;
class TagParserState;
class InlineTagParserState;
class TextParserState;
class Tokenizer;
class Language;
class MarkupBuilder;
class AbstractListIndexGenerator;
class NumericListIndexGenerator;
class MultiLevelNumericListIndexGenerator;
class SingleLevelListIndexGenerator;
class CharRangeIndexGenerator;
class RomanNumbersIndexGenerator;
class ListItemsCounter;
class ListFormat;
struct ImageSize;

typedef std::auto_ptr<AbstractGenerator> AbstractGeneratorPtr;
typedef std::auto_ptr<AbstractParserState> AbstractParserStatePtr;

#define MAX_INT_SIZE ((size_t)12)

/**
 * Set of supported generator types.
 */
enum GeneratorTypes {
	GENERATOR_HTML, GENERATOR_TEX
};

/**
 * Set of supported alignments.
 */
enum Alignments {
	ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER, ALIGN_JUSTIFY, ALIGN_DEFAULT
};

/**
 * Set of supported size units.
 */
enum Units {
	SIZE_PX,
	SIZE_PT,
	SIZE_PC,
	SIZE_PR,
	SIZE_CM,
	SIZE_MM,
	SIZE_IN,
	SIZE_EM,
	SIZE_EX
};

/**
 * Types of quotation marks.
 */
enum QuoteTypes {
	QUOTE_NONE,
	QUOTE_FRENCH_OPENED,
	QUOTE_FRENCH_CLOSED,
	QUOTE_GERMAN_OPENED,
	QUOTE_GERMAN_CLOSED,
	QUOTE_ENGLISH_OPENED,
	QUOTE_ENGLISH_CLOSED,
	QUOTE_ENGLISH_SINGLE_OPENED,
	QUOTE_ENGLISH_SINGLE_CLOSED
};

/**
 * Constants for the types of the lines within <img> tag.
 * 1. URL of the image.
 * 2. Alternative text.
 * 3. Ignore all the rest.
 */
enum ImageTagLines {
	IMAGE_TAG_LINE_URL, IMAGE_TAG_LINE_ALT, IMAGE_TAG_LINE_IGNORE
};

/**
 * Returns Alignment value by its string code.
 *
 * al|лв      - ALIGN_LEFT
 * ar|пв      - ALIGN_RIGHT
 * ac|цв      - ALIGN_CENTER
 * aj|шв      - ALIGN_JUSTIFY
 * <other>    - ALIGN_DEFAULT
 *
 * @param	alignment	String representation of the alignment.
 *
 */
Alignments parse_alignment(const std::wstring& alignment);

/**
 * Returns Alignment value by its string code.
 *
 * al      - ALIGN_LEFT
 * ar      - ALIGN_RIGHT
 * ac      - ALIGN_CENTER
 * aj      - ALIGN_JUSTIFY
 * <other>    - ALIGN_DEFAULT
 *
 * @param	alignment	String representation of the alignment.
 *
 */
Alignments parse_alignment(const std::string& alignment);

/**
 * Reads the sequence of digit converting it to an integer value.
 *
 * @param	arg 	The string which the integer is read from.
 * @param	from	The index of the first char to parse.
 * @param	number	The parsed number.
 * @return	Number of chars parsed.
 */
size_t read_number(const std::wstring& arg, size_t from, int& number);

/**
 * Reads the unit value form string.
 *
 * @param	arg 	the string which the using is read from.
 * @param	from	the index of the first char to parse.
 * @param	unit	where to store the result.
 * @return	number of chars parsed.
 */
size_t read_unit(const std::wstring& arg, size_t from, Units& unit);

/**
 * Returns a string value of a unit.
 *
 * @param	u	unit value.
 * @return	string value of the unit.
 */
const char* unit_str(Units u);

/**
 * Parses STML from the in stream and generates output to the out stream
 * using specified generator.
 */
void parse(std::istream& in, std::ostream& out, GeneratorTypes generator_type);

}

#endif /* STML_H_ */
