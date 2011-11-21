#include <cassert>
#include "markup_builder_test.hpp"
#include "../libstml/include/markup_builder.hpp"
#include <sstream>

using namespace std;
using namespace stml;

void markup_builder_test() {
    MarkupBuilder bld;

    bld << L'a' << L'b';
    bld << L"cd...";

    assert(bld.get_text() == L"abcd...");

    bld.first_char().prepend("<b>");
    bld.last_char().append("</b>");
    bld[1].prepend('_');
    bld.substitute(4, 3, "&some_entity;");

    stringstream out;
    bld.write(out);
    cout << out.str() << endl;

    string out_str;
    bld.append(out_str);

    cout << out_str << endl;
    assert(out.str() == out_str);

    MarkupBuilder bld2(bld);

    stringstream out2;
    bld2.write(out2);
    assert(out.str() == out2.str());
}


void markup_builder_merge() {
	MarkupBuilder bld1, bld2;

	bld1 << L"abc";
	bld2 << L"def";

	bld1 << bld2;

	stringstream out;

	bld1.write(out);

	string s = out.str();
	const char* s_char = s.c_str();

	assert(s == "abcdef");
}
