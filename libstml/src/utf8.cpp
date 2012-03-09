#include <cstring>
#include "../include/utf8.hpp"

using namespace std;
using namespace stml;

size_t stml::read_utf8_char(const char* in, size_t at, wchar_t* out) {
	const unsigned char* utf8byte = (unsigned char*)(in + at);

	if ((*utf8byte & LEADING_BIT_CHAR) == 0) {
		*out = (wchar_t)(*utf8byte);
		return 1;
	}
	else {
		*out = L'\0';

		size_t utf8len = 0;
		unsigned int utf8byte_bitmask = ~LEADING_BIT_CHAR;
		unsigned int len_bit = LEADING_BIT_CHAR;

		while ((*utf8byte & len_bit) != 0) {
			len_bit >>= 1;
			utf8byte_bitmask &= ~len_bit;

			++utf8len;
			if (utf8len > 6) {
				return 0;
			}
		}

		if (utf8len > 1) {

			size_t shift = utf8len * BITS_PER_UTF8_BYTE;
			while (shift > 0) {
				shift -= BITS_PER_UTF8_BYTE;
				*out |= (*utf8byte & utf8byte_bitmask) << shift;
				++utf8byte;
				utf8byte_bitmask = SUCCEEDING_UTF8_BYTE_BITMASK;
			}

			return utf8len;
		}
		else {
			return 0;
		}
	}
}

size_t stml::write_utf8_char(wchar_t in, char* out) {
	if ((unsigned int)in < LEADING_BIT_CHAR) {
		*out = (char)in;
		return 1;
	}
	else {
		unsigned int bit = LEADING_BIT_WCHAR;
		while ((bit & in) == 0) {
			bit >>= 1;
		}

		size_t utf8_len = 0;
		while (bit != 0) {
			bit >>= 1;
			++utf8_len;
		}

		utf8_len = (utf8_len / BITS_PER_UTF8_BYTE) + 1;

		unsigned int utf8size_bit = LEADING_BIT_CHAR;
		unsigned int first_byte_prefix = 0;

		for (size_t i = 0; i < utf8_len; ++i) {
			unsigned int shift = BITS_PER_UTF8_BYTE * (utf8_len - i - 1);
			unsigned int utf8byte_payload_bitmask = SUCCEEDING_UTF8_BYTE_BITMASK << shift;

			unsigned int utf8byte_payload = (in & utf8byte_payload_bitmask) >> shift;

			out[i] = (char)(LEADING_BIT_CHAR | utf8byte_payload);

			first_byte_prefix |= utf8size_bit;
			utf8size_bit >>= 1;
		}

		out[0] = (char)(first_byte_prefix | out[0]);

		return utf8_len;
	}
}

void stml::append_wchar_to_utf8(string& target, const wchar_t* str) {
	char c[MAX_UTF8_CHAR_LENGTH];

	size_t i = 0;
	while (str[i]) {
		memset(c, 0, MAX_UTF8_CHAR_LENGTH * sizeof(char));
		write_utf8_char(str[i], c);

		target += c;

		++i;
	}
}

size_t stml::write_utf8_char(wchar_t in, std::ostream& out) {
	char c[MAX_UTF8_CHAR_LENGTH];
	memset(c, 0, MAX_UTF8_CHAR_LENGTH * sizeof(char));

	size_t bytes_written = stml::write_utf8_char(in, c);
	out << c;

	return bytes_written;
}

