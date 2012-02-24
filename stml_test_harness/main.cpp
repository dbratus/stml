#include "markup_builder_test.hpp"
#include "ru_language_test.hpp"
#include "list_items_counter_test.hpp"
#include "list_index_generators_test.hpp"
#include "list_format_test.hpp"

int main() {
    markup_builder_test();
    markup_builder_merge();
    ru_language_test();
    list_items_counter_test();
    multi_level_list_index_generator();
    single_level_numeric_list_index_generator();
    char_range_list_index_generator();
    roman_list_index_generator();
    list_format();

    return 0;
}
