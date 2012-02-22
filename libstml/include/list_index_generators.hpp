#ifndef LIST_INDEX_GENERATORS_HPP_
#define LIST_INDEX_GENERATORS_HPP_

#include <vector>
#include <cstring>

namespace stml {

/**
 * Base class for all list index generators.
 */
class AbstractListIndexGenerator {
protected:
	static const size_t BUFFER_SIZE = 50;

	wchar_t buffer[BUFFER_SIZE];

	inline void init_buffer() { memset(buffer, 0, BUFFER_SIZE * sizeof(wchar_t)); }

	virtual ~AbstractListIndexGenerator() { }

public:

	/**
	 * Generates an index for the item with the specified path.
	 *
	 * @param item_path Path to the item.
	 * @return Pointer to wchar_t string containing the index.
	 *         The pointer is valid until the next call to the method.
	 * @throws StmlException If the index is too large or too deep.
	 */
	virtual const wchar_t* generate_index(const std::vector<int>& item_path) = 0;
};

class NumericListIndexGenerator : public AbstractListIndexGenerator {
protected:
	NumericListIndexGenerator();
	virtual ~NumericListIndexGenerator() { }

	/**
	 * Writes a non-negative integer value to the buffer at the specified position.
	 *
	 * @param at     The position.
	 * @param number The value.
	 * @return The position right after the written number or -1 if overflow occurred.
	 */
	int write_to_buffer(int at, int number);
};

/**
 * Generates numeric multilevel indexes like: 1. 1.1. 1.2. 2. 2.1. 2.2. ...
 */
class MultiLevelNumericListIndexGenerator : public NumericListIndexGenerator {
	static const int MAX_INDEX = 999;

public:
	MultiLevelNumericListIndexGenerator();
	virtual ~MultiLevelNumericListIndexGenerator() { }

	const wchar_t* generate_index(const std::vector<int>& item_path);
};

/**
 * Generates numeric indexes.
 */
class SingleLevelListIndexGenerator : public NumericListIndexGenerator {
	wchar_t left_bracket;
	wchar_t right_bracket;

public:
	SingleLevelListIndexGenerator(wchar_t left_bracket, wchar_t right_bracket);
	virtual ~SingleLevelListIndexGenerator() { }

	const wchar_t* generate_index(const std::vector<int>& item_path);
};

class BracketedIndexGenerator : public AbstractListIndexGenerator {
	wchar_t left_bracket;
	wchar_t right_bracket;

protected:

	/**
	 * Writes a positive integer value to the buffer at the specified position
	 * using the format provided by implementor.
	 *
	 * @param at     The position.
	 * @param number The value.
	 * @return The position right after the written number or -1 if overflow occurred.
	 */
	virtual int write_to_buffer(int at, int number) = 0;

	BracketedIndexGenerator(wchar_t left_bracket, wchar_t right_bracket);
	virtual ~BracketedIndexGenerator() { }

public:

	const wchar_t* generate_index(const std::vector<int>& item_path);
};

/**
 * Generates indexes using a range of characters. For example, say the range is A-C,
 * then the indexes will be: A B C AA AB AC BA BB BC CA CB CC AAA ...
 */
class CharRangeIndexGenerator : public BracketedIndexGenerator {
	wchar_t range_start;
	wchar_t range_end;

protected:
	int write_to_buffer(int at, int number);

public:
	CharRangeIndexGenerator(wchar_t left_bracket, wchar_t right_bracket, wchar_t range_start, wchar_t range_end);
	virtual ~CharRangeIndexGenerator() { }
};

/**
 * Generates Roman number indexes using capitals or lower case.
 */
class RomanNumbersIndexGenerator : public BracketedIndexGenerator {
	bool use_capitals;

protected:
	int write_to_buffer(int at, int number);

public:
	RomanNumbersIndexGenerator(wchar_t left_bracket, wchar_t right_bracket, bool use_capitals);
	virtual ~RomanNumbersIndexGenerator() { }
};

}

#endif /* LIST_INDEX_GENERATORS_HPP_ */
