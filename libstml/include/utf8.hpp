#ifndef UTF8_HPP_
#define UTF8_HPP_

#define LEADING_BIT_CHAR (unsigned int)(0x80)
#define LEADING_BIT_WCHAR (unsigned int)(0x80000000)
#define SUCCEEDING_UTF8_BYTE_BITMASK (unsigned int)(0x3F)
#define BITS_PER_UTF8_BYTE (size_t)(6)
#define MAX_UTF8_CHAR_LENGTH (size_t)(4)

#include <string>

namespace stml {

/**
 * Reads one UTF8 char from 'in' starting at position 'at' into 'out'.
 *
 * @param	in	Input byte stream.
 * @param	at	Position the char to be read from.
 * @param	out	Destination the char code point to be written to.
 *
 * @return	Number of bytes read from 'in'; zero if the sequence is not correct UTF8.
 */
size_t read_utf8_char(const char* in, size_t at, wchar_t* out);

/**
 * Writes the char with the code point 'in' into 'out'.
 * WARNING: 'out' must have at least MAX_UTF8_CHAR_LENGTH length.
 *
 * @param	in	The code point to be written.
 * @param	out	Destination the char is to be written to.
 *
 * @return	Number of bytes written to 'out'.
 */
size_t write_utf8_char(wchar_t in, char* out);

/**
 * Appends a wide char string to a char string in UTF8.
 *
 * @param	target	The string which the 'str' must be appended to.
 * @param	str		The string that must be appended.
 */
void append_wchar_to_utf8(std::string& target, const wchar_t* str);

}

#endif /* UTF8_HPP_ */
