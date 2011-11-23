#ifndef LIST_ITEMS_COUNTER_HPP_
#define LIST_ITEMS_COUNTER_HPP_

#include <vector>

namespace stml {

class ListItemsCounter {
	static const int DEFAULT_COUNTER_SIZE = 10;

	std::vector<int> counter;

public:

	ListItemsCounter();

	/**
	 * Resets the counter to the initial state.
	 */
	void reset();

	/**
	 * Increments the counter on the specified level.
	 */
	void increment(int level);

	/**
	 * Path to the current item. The size of the vector is the level of
	 * the item. Each value is the index (starting from 1) of the ancestor
	 * of the current item on the corresponding level.
	 */
	inline const std::vector<int>& current_item_path() const {
		return counter;
	}
};

}


#endif /* LIST_ITEMS_COUNTER_HPP_ */
