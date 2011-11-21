#ifndef MARKUP_BUILDER_HPP_
#define MARKUP_BUILDER_HPP_

#include <iostream>
#include <string>
#include <vector>

namespace stml {

class MarkupBuilder {
    class Char {
        wchar_t itself;
        std::vector<char> preceding;
        std::vector<char> substituting;
        std::vector<char> following;

        static void put_char(wchar_t c, std::ostream& out);

    public:

        inline Char() { }
        inline Char(const Char& c) { *this = c; }

        inline Char& operator =(const Char& c) {
            itself = c.itself;
            preceding = c.preceding;
            substituting = c.substituting;
            following = c.following;
            return *this;
        }

        inline void clear() {
            itself = L'\0';
            preceding.clear();
            substituting.clear();
            following.clear();
        }

        inline void set(wchar_t c) {
        	itself = c;
        }

        inline void prepend(const char* str) {
            size_t len = std::char_traits<char>::length(str);

            if (len > 0) {
                for(size_t i = len; i > 0; --i) {
                    prepend(str[i - 1]);
                }
            }
        }

        inline void prepend(char c) {
            preceding.push_back(c);
        }

        inline void append(const char* str) {
            size_t len = std::char_traits<char>::length(str);

            if (len > 0) {
                for(size_t i = 0; i < len; ++i) {
                    append(str[i]);
                }
            }
        }

        inline void append(char c) {
            following.push_back(c);
        }

        inline void substitute(char c) {
            substituting.push_back(c);
        }

        void write(std::ostream& out) const;
    };

private:

    static const int DEFAULT_BUFFER_SIZE = 1024;

    std::wstring text;
    std::vector<Char> buffer;
    size_t chars_in_buffer;

    void ensure_storage_for_next_char();

public:

    MarkupBuilder();
    MarkupBuilder(const MarkupBuilder& builder);

    MarkupBuilder& operator =(const MarkupBuilder& builder);
    Char& operator [](size_t index);
    MarkupBuilder& operator <<(const wchar_t* str);
    MarkupBuilder& operator <<(wchar_t c);
    MarkupBuilder& operator <<(const MarkupBuilder& markup);

    void substitute(size_t index, size_t length, const char* str);

    void clear();
    bool empty() const;
    Char& first_char();
    Char& last_char();
    Char& next_char();
    size_t last_char_index();

    const std::wstring& get_text() const;

    void write(std::ostream& out) const;
    void append(std::string& str) const;
};

}

#endif /* MARKUP_BUILDER_HPP_ */
