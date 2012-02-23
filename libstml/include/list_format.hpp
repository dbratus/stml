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
	AbstractListIndexGenerator* default_generator;

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
	inline AbstractListIndexGenerator* generator(int level) const {
		return (level < MAX_GENERATORS) ? generators[level] : default_generator;
	}
};

}

#endif /* LIST_FORMAT_HPP_ */
