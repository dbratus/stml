#include "../libstml/include/list_items_counter.hpp"

#include "list_items_counter_test.hpp"
#include <iostream>
#include <cassert>

using namespace std;
using namespace stml;

void list_items_counter_test() {
	ListItemsCounter counter;

	counter.increment(1);
	assert(counter.current_item_path().size() == 1 && counter.current_item_path()[0] == 1);

	counter.increment(1);
	assert(counter.current_item_path().size() == 1 && counter.current_item_path()[0] == 2);

	counter.increment(2);
	assert(
			counter.current_item_path().size() == 2 &&
			counter.current_item_path()[0] == 2 &&
			counter.current_item_path()[1] == 1
	);

	counter.increment(2);
	assert(
			counter.current_item_path().size() == 2 &&
			counter.current_item_path()[0] == 2 &&
			counter.current_item_path()[1] == 2
	);

	counter.increment(1);
	assert(counter.current_item_path().size() == 1 && counter.current_item_path()[0] == 3);
}


