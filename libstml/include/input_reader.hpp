#ifndef INPUT_READER_H_
#define INPUT_READER_H_

#include <iostream>
#include <string>

namespace stml {

class InputReader {
    static const size_t DEFAULT_BUFFER_SIZE = 1024;

    std::istream* in;
    std::string line;

    size_t line_bytes_left;
    const char* line_pointer;

public:
    InputReader(std::istream* in);

    wchar_t next_char();
    bool next_line();
};

}

#endif /* INPUT_READER_H_ */
