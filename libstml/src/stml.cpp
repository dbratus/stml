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

Alignments stml::parse_alignment(const wstring& alignment) {
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

Alignments stml::parse_alignment(const string& alignment) {
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

const char* stml::unit_str(Units u) {
	switch (u) {
	case SIZE_PX:
		return "px";
	case SIZE_PT:
		return "pt";
	case SIZE_PC:
		return "pc";
	case SIZE_PR:
		return "%";
	case SIZE_CM:
		return "cm";
	case SIZE_MM:
		return "mm";
	case SIZE_IN:
		return "in";
	case SIZE_EM:
		return "em";
	case SIZE_EX:
		return "ex";
	default:
		return "";
	}
}

size_t stml::read_unit(const std::wstring& arg, size_t from, Units& unit) {
	wchar_t u[] = { L'\0', L'\0', L'\0' };

	u[0] = arg[from];
	u[1] = arg[from + 1];

	if (u[0] == L'%') {
		unit = SIZE_PR;
		return 1;
	} else if (char_traits<wchar_t>::compare(u, L"px", 2) == 0) {
		unit = SIZE_PX;
		return 2;
	} else if (char_traits<wchar_t>::compare(u, L"pt", 2) == 0) {
		unit = SIZE_PT;
		return 2;
	} else if (char_traits<wchar_t>::compare(u, L"pc", 2) == 0) {
		unit = SIZE_PC;
		return 2;
	} else if (char_traits<wchar_t>::compare(u, L"cm", 2) == 0) {
		unit = SIZE_CM;
		return 2;
	} else if (char_traits<wchar_t>::compare(u, L"mm", 2) == 0) {
		unit = SIZE_MM;
		return 2;
	} else if (char_traits<wchar_t>::compare(u, L"in", 2) == 0) {
		unit = SIZE_IN;
		return 2;
	} else if (char_traits<wchar_t>::compare(u, L"em", 2) == 0) {
		unit = SIZE_EM;
		return 2;
	} else if (char_traits<wchar_t>::compare(u, L"ex", 2) == 0) {
		unit = SIZE_EX;
		return 2;
	}

	return 0;
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
