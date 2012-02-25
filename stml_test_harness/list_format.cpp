#include "list_format.hpp"
#include "../libstml/include/list_index_generators.hpp"
#include "../libstml/include/list_format.hpp"
#include "../libstml/include/list_items_counter.hpp"

#include <iostream>
#include <string>

using namespace std;
using namespace stml;

void list_format() {
	ListFormat fmt;
	ListItemsCounter cnt;
	wstring index;

	fmt.set(L"#./.#./#)/(I)/i./(a-z)");

	for (int i = 0; i < 6; ++i) {
		for (int j = i; j < 6; ++j) {
			cnt.increment(i + 1);

			index = fmt.generator(i + 1)->generate_index(cnt.current_item_path());
			wcout << index << endl;
		}
	}
}


