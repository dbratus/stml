#include "../include/stml.hpp"
#include "../include/abstract_generator.hpp"
#include "../include/input_reader.hpp"
#include "../include/parser_state.hpp"
#include "../include/parser.hpp"

using namespace stml;
using namespace std;

void stml::parse(istream& in, ostream& out, GeneratorTypes generator_type) {
	Parser parser(generator_type);
	parser.parse(in, out);
}

Alignment stml::parse_alignment(const wstring& alignment) {
	if (alignment == L"al" || alignment == L"лв") {
		return ALIGN_LEFT;
	} else if (alignment == L"ar" || alignment == L"пв") {
		return ALIGN_RIGHT;
	} else if (alignment == L"ac" || alignment == L"цв") {
		return ALIGN_CENTER;
	} else if (alignment == L"aj" || alignment == L"шв") {
		return ALIGN_JUSTIFY;
	} else {
		return ALIGN_DEFAULT;
	}
}

Alignment stml::parse_alignment(const string& alignment) {
	if (alignment == "al") {
		return ALIGN_LEFT;
	} else if (alignment == "ar") {
		return ALIGN_RIGHT;
	} else if (alignment == "ac") {
		return ALIGN_CENTER;
	} else if (alignment == "aj") {
		return ALIGN_JUSTIFY;
	} else {
		return ALIGN_DEFAULT;
	}
}

size_t stml::read_number(const wstring& arg, size_t from, int& number) {
	wchar_t str[MAX_INT_SIZE];
	size_t str_i = 0;
	size_t arg_i = from;
	size_t len = arg.length();

	if (arg_i >= len) {
		return 0;
	}

	memset(str, 0, sizeof(wchar_t) * MAX_INT_SIZE);

	while (str_i < MAX_INT_SIZE && arg_i < len && arg[arg_i] >= L'0' && arg[arg_i] <= L'9') {
		str[str_i++] = arg[arg_i++];
	}

	swscanf(str, L"%d", &number);

	return str_i;
}
