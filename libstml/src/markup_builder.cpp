#include "../include/stml.hpp"
#include "../include/utf8.hpp"
#include "../include/markup_builder.hpp"
#include "../include/stml_exception.hpp"

#include <sstream>
#include <stdexcept>

using namespace std;
using namespace stml;

void MarkupBuilder::Char::put_char(wchar_t c, ostream& out) {
    char out_buffer[MAX_UTF8_CHAR_LENGTH + 1];

    size_t bytes_written = write_utf8_char(c, out_buffer);
    if (bytes_written == 0) {
    	throw StmlException(StmlException::CHARACTER_CANNOT_BE_CONVERTED_TO_OUTPUT_FORMAT);
    }
    out_buffer[bytes_written] = L'\0';

    out << out_buffer;
}

void MarkupBuilder::Char::write(ostream& out) const {
    if (itself) {
        vector<char>::const_reverse_iterator prec;
        for (prec = preceding.rbegin(); prec < preceding.rend(); ++prec) {
            out << *prec;
        }

        if (substituting.empty()) {
        	if (itself != L'\0') {
            	put_char(itself, out);
        	}
        }
        else {
            char c = substituting.back();
            if (c != '\0') {
                out << c;
            }
        }

        vector<char>::const_iterator foll;
        for (foll = following.begin(); foll < following.end(); ++foll) {
            out << *foll;
        }
    }
}

MarkupBuilder::MarkupBuilder() {
    text.reserve(DEFAULT_BUFFER_SIZE);
    buffer.resize(DEFAULT_BUFFER_SIZE);
    chars_in_buffer = 0;
}

MarkupBuilder::MarkupBuilder(const MarkupBuilder& builder) {
    *this = builder;
}

MarkupBuilder& MarkupBuilder::operator =(const MarkupBuilder& builder) {
    text = builder.text;
    buffer = builder.buffer;
    chars_in_buffer = builder.chars_in_buffer;
    return *this;
}

MarkupBuilder::Char& MarkupBuilder::operator [](size_t index) {
    if (index >= chars_in_buffer) {
        throw out_of_range("index");
    }

    return buffer[index];
}

void MarkupBuilder::ensure_storage_for_next_char() {
    size_t buf_size = buffer.size();
    if (buf_size == chars_in_buffer + 1) {
        buffer.resize(buf_size * 2);
    }
}

MarkupBuilder& MarkupBuilder::operator <<(const wchar_t* str) {
    size_t len = char_traits<wchar_t>::length(str);
    for(size_t i = 0; i < len; ++i) {
        *this << str[i];
    }

    return *this;
}

MarkupBuilder& MarkupBuilder::operator <<(wchar_t c) {
    buffer[chars_in_buffer].set(c);
    text += c;

    ensure_storage_for_next_char();

    ++chars_in_buffer;
    buffer[chars_in_buffer].clear();

    return *this;
}

MarkupBuilder& MarkupBuilder::operator <<(const MarkupBuilder& markup) {

	wstring markup_text = markup.get_text();
	size_t len = markup_text.length();

	for (size_t i = 0; i < len; ++i) {
		text += markup_text[i];
		buffer[chars_in_buffer] = markup.buffer[i];

		ensure_storage_for_next_char();
		++chars_in_buffer;
		buffer[chars_in_buffer].clear();
	}

	return *this;
}

void MarkupBuilder::substitute(size_t index, size_t length, const char* str) {
    if (length == 0) {
        throw logic_error("length");
    }

    if (index + length > chars_in_buffer) {
        throw out_of_range("length");
    }

    size_t len = char_traits<char>::length(str);
    if (len == 0) {
        return;
    }

    for (size_t i = 1; i < length; ++i) {
        (*this)[index + i].substitute('\0');
    }

    (*this)[index].substitute(str[0]);

    for (size_t i = 1; i < len; ++i) {
        (*this)[index].append(str[i]);
    }
}

void MarkupBuilder::clear() {
    chars_in_buffer = 0;
    buffer[0].clear();
    text.clear();
}

bool MarkupBuilder::empty() const {
    return chars_in_buffer == 0;
}

MarkupBuilder::Char& MarkupBuilder::first_char() {
    if (empty()) {
        throw out_of_range("");
    }

    return buffer[0];
}

MarkupBuilder::Char& MarkupBuilder::last_char() {
    if (empty()) {
        throw out_of_range("");
    }

    return buffer[chars_in_buffer - 1];
}

size_t MarkupBuilder::last_char_index() {
    if (empty()) {
        throw out_of_range("");
    }

    return chars_in_buffer - 1;
}

MarkupBuilder::Char& MarkupBuilder::next_char() {
    return buffer[chars_in_buffer];
}

const wstring& MarkupBuilder::get_text() const {
    return text;
}

void MarkupBuilder::write(ostream& out) const {
    for(size_t i = 0; i < chars_in_buffer; ++i) {
        buffer[i].write(out);
    }
}

void MarkupBuilder::append(string& str) const {
    stringstream out;
    write(out);
    str += out.str();
}
