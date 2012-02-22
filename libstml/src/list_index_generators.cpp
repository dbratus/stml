#include "../include/list_index_generators.hpp"
#include "../include/stml_exception.hpp"

#include <cstdio>

using namespace std;
using namespace stml;

NumericListIndexGenerator::NumericListIndexGenerator()
	: AbstractListIndexGenerator() {
}

int NumericListIndexGenerator::write_to_buffer(int at, int number) {
	if (number < 0) {
		return -1;
	}

	static const int radix = 10;

	int num = number;
	int last = at - 1;
	while (num) {
		++last;
		num /= radix;

		if (last == BUFFER_SIZE - 1) {
			return -1;
		}
	}

	int i = last;
	while (number) {
		buffer[i--] = L'0' + (wchar_t)(number % radix);
		number /= radix;
	}

	return last + 1;
}

MultiLevelNumericListIndexGenerator::MultiLevelNumericListIndexGenerator()
	: NumericListIndexGenerator() {
}

const wchar_t* MultiLevelNumericListIndexGenerator::generate_index(const vector<int>& item_path) {
	init_buffer();
	int i = 0;

	for(vector<int>::const_iterator lvl = item_path.begin(); lvl != item_path.end(); ++lvl) {
		i = write_to_buffer(i, *lvl);

		if (i < 0) {
			throw StmlException(StmlException::LIST_INDEX_OVERFLOW);
		}

		buffer[i++] = L'.';

		if (i == BUFFER_SIZE) {
			throw StmlException(StmlException::LIST_INDEX_OVERFLOW);
		}
	}

	return buffer;
}

SingleLevelListIndexGenerator::SingleLevelListIndexGenerator(wchar_t left_bracket, wchar_t right_bracket)
	: NumericListIndexGenerator() {

	this->left_bracket = left_bracket;
	this->right_bracket = right_bracket;
}

const wchar_t* SingleLevelListIndexGenerator::generate_index(const vector<int>& item_path) {
	init_buffer();
	int i = 0;

	if (left_bracket) {
		buffer[i++] = left_bracket;
	}

	if (!item_path.empty()) {
		i = write_to_buffer(i, *(item_path.end() - 1));
	}

	if (right_bracket) {
		buffer[i++] = right_bracket;
	}

	return buffer;
}

BracketedIndexGenerator::BracketedIndexGenerator(
		wchar_t left_bracket,
		wchar_t right_bracket)
	: AbstractListIndexGenerator() {
	this->left_bracket = left_bracket;
	this->right_bracket = right_bracket;
}

const wchar_t* BracketedIndexGenerator::generate_index(const vector<int>& item_path) {
	init_buffer();
	int i = 0;

	if (left_bracket) {
		buffer[i++] = left_bracket;
	}

	if (!item_path.empty()) {
		i = write_to_buffer(i, *(item_path.end() - 1));

		if (i < 0 || i == BUFFER_SIZE) {
			throw StmlException(StmlException::LIST_INDEX_OVERFLOW);
		}
	}

	if (right_bracket) {
		buffer[i++] = right_bracket;

		if (i == BUFFER_SIZE) {
			throw StmlException(StmlException::LIST_INDEX_OVERFLOW);
		}
	}

	return buffer;
}

CharRangeIndexGenerator::CharRangeIndexGenerator(
		wchar_t left_bracket,
		wchar_t right_bracket,
		wchar_t range_start,
		wchar_t range_end)
	: BracketedIndexGenerator(left_bracket, right_bracket) {
	this->range_start = range_start;
	this->range_end = range_end;
}

int CharRangeIndexGenerator::write_to_buffer(int at, int number) {
	if (number < 1) {
		return -1;
	}

	int radix = range_end - range_start + 1;
	if (radix <= 0) {
		return -1;
	}

	--number;

	int num = number;
	int last = at - 1;
	while (num >= 0) {
		++last;
		num = num / radix - 1;

		if (last == BUFFER_SIZE - 1) {
			return -1;
		}
	}

	int i = last;
	while (number >= 0) {
		buffer[i--] = range_start + (wchar_t)(number % radix);
		number = number / radix - 1;
	}

	return last + 1;
}

RomanNumbersIndexGenerator::RomanNumbersIndexGenerator(wchar_t left_bracket, wchar_t right_bracket, bool use_capitals)
	: BracketedIndexGenerator(left_bracket, right_bracket) {
	this->use_capitals = use_capitals;
}

int RomanNumbersIndexGenerator::write_to_buffer(int at, int number) {
	if (number < 1) {
		return -1;
	}

	static const wchar_t* upper_case_digits[] =
		{ L"I", L"IV", L"V", L"IX", L"X", L"XL", L"L", L"XC", L"C", L"CD", L"D", L"CM", L"M" };
	static const wchar_t* lower_case_digits[] =
		{ L"i", L"iv", L"v", L"ix", L"x", L"xl", L"l", L"xc", L"c", L"cd", L"d", L"cm", L"m" };
	static const int values[] = { 1, 4, 5, 9, 10, 40, 50, 90, 100, 400, 500, 900, 1000 };
	static const int digits_count = sizeof(values) / sizeof(int);

	const wchar_t** digits = (use_capitals) ? upper_case_digits : lower_case_digits;

	int current_digit = digits_count - 1;
	while (number) {
		if (values[current_digit] <= number) {
			const wchar_t* dp = digits[current_digit];

			while(buffer[at] = *dp++) {
				++at;

				if (at == BUFFER_SIZE) {
					return -1;
				}
			}

			number -= values[current_digit];
		} else {
			--current_digit;
		}
	}

	return at;
}
