#include "../include/stml.hpp"
#include "../include/utf8.hpp"
#include "../include/input_reader.hpp"
#include "../include/stml_exception.hpp"

using namespace stml;
using namespace std;

InputReader::InputReader(istream* in) {
	this->in = in;
	line.reserve(DEFAULT_BUFFER_SIZE);
}

wchar_t InputReader::next_char() {
	if (line_bytes_left) {
		wchar_t c;
		size_t bytes_read = read_utf8_char(line_pointer, 0, &c);
		if (bytes_read == 0) {
			throw StmlException(StmlException::CHARACTER_CANNOT_BE_CONVERTED_TO_INTERNAL_FORMAT);
		}

		line_pointer += bytes_read;
		line_bytes_left -= bytes_read;

		return c;
	}
	else {
		return L'\0';
	}
}

bool InputReader::next_line() {
	if (!in->eof()){
		getline(*in, line);

		line_bytes_left = line.length();
		line_pointer = line.data();

		return true;
	}
	else {
		return false;
	}
}
