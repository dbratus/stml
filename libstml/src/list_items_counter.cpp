#include "../include/list_items_counter.hpp"

using namespace std;
using namespace stml;

ListItemsCounter::ListItemsCounter() {
	counter.reserve(DEFAULT_COUNTER_SIZE);
}

void ListItemsCounter::reset() {
	counter.clear();
}

void ListItemsCounter::increment(int level) {
	if (counter.size() > level) {
		counter.erase(counter.end() - (counter.size() - level));
	} else if (counter.size() < level) {
		counter.insert(counter.end(), level - counter.size(), 0);
	}

	counter[level - 1] += 1;
}
