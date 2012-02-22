#include "list_index_generators_test.hpp"
#include "../libstml/include/list_index_generators.hpp"

#include "roman_indexes.hpp"

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>
#include <string>

using namespace std;
using namespace stml;

void multi_level_list_index_generator() {
	auto_ptr<MultiLevelNumericListIndexGenerator> generator(new MultiLevelNumericListIndexGenerator());
	vector<int> item_path;
	wstring index;

	item_path.push_back(1);
	index = generator->generate_index(item_path);
	assert(index == L"1.");

	item_path.push_back(2);
	index = generator->generate_index(item_path);
	assert(index == L"1.2.");

	item_path.push_back(3);
	index = generator->generate_index(item_path);
	assert(index == L"1.2.3.");
}

void single_level_numeric_list_index_generator() {
	auto_ptr<SingleLevelListIndexGenerator> generator(new SingleLevelListIndexGenerator(L'(', L')'));
	vector<int> item_path;
	wstring index;

	item_path.push_back(1);
	index = generator->generate_index(item_path);
	assert(index == L"(1)");

	item_path.push_back(2);
	index = generator->generate_index(item_path);
	assert(index == L"(2)");

	item_path.push_back(3);
	index = generator->generate_index(item_path);
	assert(index == L"(3)");

	generator.reset(new SingleLevelListIndexGenerator(L'\0', L'.'));
	item_path.clear();
	item_path.push_back(1);

	index = generator->generate_index(item_path);
	assert(index == L"1.");

	item_path.push_back(2);
	index = generator->generate_index(item_path);
	assert(index == L"2.");

	item_path.push_back(3);
	index = generator->generate_index(item_path);
	assert(index == L"3.");
}

void char_range_list_index_generator() {
	auto_ptr<CharRangeIndexGenerator> generator(new CharRangeIndexGenerator(L'(', L')', 'A', 'C'));
	vector<int> item_path;
	wstring index;

	static const wchar_t* indexes[] =
		{ L"(A)", L"(B)", L"(C)",
		  L"(AA)", L"(AB)", L"(AC)",
		  L"(BA)", L"(BB)", L"(BC)",
		  L"(CA)", L"(CB)", L"(CC)",
		  L"(AAA)",};

	for (int i = 0; i < sizeof(indexes) / sizeof(wchar_t*); ++i) {
		item_path.push_back(i + 1);
		index = generator->generate_index(item_path);
		assert(index == indexes[i]);
	}
}

void roman_list_index_generator() {
	auto_ptr<RomanNumbersIndexGenerator> generator(new RomanNumbersIndexGenerator(L'(', L')', true));
	vector<int> item_path;
	wstring index;

	item_path.push_back(0);

	//wcout << endl << "static const wchar_t* roman_indexes[] = { ";

	for (int i = 1; i < 3000; ++i) {
		item_path[0] = i;
		index = generator->generate_index(item_path);

		assert(index == roman_indexes[i - 1]);

		//if (i > 1) {
		//	wcout << ", ";
		//}

		//wcout << "L\"" << index << "\"" << endl;
	}

	//wcout << "};";
}
