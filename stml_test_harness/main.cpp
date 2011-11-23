#include "markup_builder_test.hpp"
#include "ru_language_test.hpp"
#include "list_items_counter_test.hpp"

int main() {
    markup_builder_test();
    markup_builder_merge();
    ru_language_test();
    list_items_counter_test();

    return 0;
}
