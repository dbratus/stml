#include "../include/stml.hpp"
#include "../include/list_format.hpp"
#include "../include/stml_exception.hpp"

using namespace std;
using namespace stml;

ListFormat::ListFormat() {
	for (size_t i = 0; i < MAX_GENERATORS; ++i) {
		generators[i] = NULL;
	}

	generators_count = 0;
}

ListFormat::~ListFormat() {
	delete_generators();
}

void ListFormat::delete_generators() {
	for (size_t i = 0; i < MAX_GENERATORS; ++i) {
		if (generators[i]) {
			delete generators[i];
			generators[i] = NULL;
		} else {
			break;
		}
	}
}

AbstractListIndexGenerator* ListFormat::create_generator(const wchar_t* format_segment) {
	size_t i = 0;
	wchar_t c;
	while (c = format_segment[i]) {
		if (c == L'#') {
			wchar_t lbr = L'\0';

			if (i > 0) {
				lbr = format_segment[i - 1];
			}

			wchar_t rbr = format_segment[i + 1];

			if (lbr == L'.' && rbr == L'.') {
				return new MultiLevelNumericListIndexGenerator();
			} else {
				return new SingleLevelListIndexGenerator(lbr, rbr);
			}
		} else if (c == L'-') {
			wchar_t lbr, start_c;

			if (i > 1) {
				lbr = format_segment[i - 2];
				start_c = format_segment[i - 1];
			} else if (i > 0) {
				lbr = L'\0';
				start_c = format_segment[i - 1];
			} else {
				throw StmlException(StmlException::INVALID_LIST_FORMAT);
			}

			wchar_t rbr, end_c;

			if (!format_segment[i + 1]) {
				throw StmlException(StmlException::INVALID_LIST_FORMAT);
			}

			if (format_segment[i + 2]) {
				rbr = format_segment[i + 2];
				end_c = format_segment[i + 1];
			} else {
				rbr = L'\0';
				end_c = format_segment[i + 1];
			}

			return new CharRangeIndexGenerator(lbr, rbr, start_c, end_c);
		} else if (c == L'I' || c == L'i') {
			wchar_t lbr = L'\0';

			if (i > 0) {
				lbr = format_segment[i - 1];
			}

			wchar_t rbr = format_segment[i + 1];

			return new RomanNumbersIndexGenerator(lbr, rbr, c == L'I');
		}

		++i;
	}

	throw StmlException(StmlException::INVALID_LIST_FORMAT);
}

void ListFormat::set(const wchar_t* format) {
	if (format == NULL) {
		throw StmlException(StmlException::INVALID_LIST_FORMAT);
	}

	delete_generators();

	static const size_t MAX_LENGTH = 4 * MAX_GENERATORS;
	static const size_t MAX_SEGMENT_LENGTH = 3;

	wchar_t lvl_fmt[MAX_SEGMENT_LENGTH + 1] = L"";
	size_t seg_i = 0;
	generators_count = 0;

	size_t i = 0;
	while (format[i]) {
		lvl_fmt[seg_i++] = format[i++];

		if (seg_i > MAX_SEGMENT_LENGTH) {
			throw StmlException(StmlException::INVALID_LIST_FORMAT);
		}

		if (i > MAX_LENGTH) {
			throw StmlException(StmlException::INVALID_LIST_FORMAT);
		}

		if (format[i] == L'/' || !format[i]) {
			lvl_fmt[seg_i] = L'\0';
			generators[generators_count++] = create_generator(lvl_fmt);

			seg_i = 0;

			if (generators_count == MAX_GENERATORS) {
				break;
			}

			if (format[i]) {
				++i;
			}
		}
	}
}

AbstractListIndexGenerator* ListFormat::generator(int level) const {
	if (level < 1 || level > generators_count) {
		throw StmlException(StmlException::FORMAT_IS_NOT_SET_FOR_LIST_LEVEL);
	}

	return generators[level - 1];
}



