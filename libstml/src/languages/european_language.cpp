#include "../../include/markup_builder.hpp"
#include "../../include/languages/european_language.hpp"

#include <stack>
#include <vector>

using namespace stml;
using namespace std;

inline void substitute_quote(
    QuoteTypes quote,
    MarkupBuilder& builder,
    size_t at,
    const map<int,string>& quotes) {

    map<int,string>::const_iterator q = quotes.find((int)quote);
    if (quotes.find(quote) != quotes.end()) {
        builder.substitute(at, 1, q->second.c_str());
    }
}

void EuropeanLanguage::process_punctuation(
    MarkupBuilder& builder,
    const char* dash,
    const map<int,string>& quotes) const {

    wstring txt = builder.get_text();
    bool quote_alt = false;

    for(size_t i = 0; i < txt.length(); ++i) {
        switch (txt[i]) {
        case L'"':
            QuoteTypes qt;
            if (i < txt.length() - 1 && is_word_char(txt[i + 1])) {
                qt = (quote_alt) ? opened_alt_quote() : opened_quote();
            }
            else {
                qt = (!quote_alt) ? closed_alt_quote() : closed_quote();
            }
            quote_alt = !quote_alt;
            substitute_quote(qt, builder, i, quotes);
            break;
        case L'-':
            if (i < txt.length() - 1 && txt[i + 1] == L' ') {
                builder.substitute(i, 1, dash);
            }
            break;
        }
    }
}
