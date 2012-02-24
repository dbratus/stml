#ifndef LIST_FORMAT_HPP_
#define LIST_FORMAT_HPP_

#include "list_index_generators.hpp"

namespace stml {

/**
 * Manages index generators for multilevel lists.
 */
class ListFormat {
	static const size_t MAX_GENERATORS = 6;

	AbstractListIndexGenerator* generators[MAX_GENERATORS];
	size_t generators_count;

	static AbstractListIndexGenerator* create_generator(const wchar_t* format_segment);
	void delete_generators();

public:

	ListFormat();
	~ListFormat();

	/**
	 * Sets format.
	 *
	 * @param format List format string.
	 */
	void set(const wchar_t* format);

	/**
	 * Returns a list index generator for the specified level.
	 *
	 * @param level Level of the item.
	 */
	AbstractListIndexGenerator* generator(int level) const;
};

}

#endif /* LIST_FORMAT_HPP_ */
