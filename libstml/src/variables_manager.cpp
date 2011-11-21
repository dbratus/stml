#include "../include/stml.hpp"
#include "../include/variables_manager.hpp"

using namespace stml;

VariablesManager::VariablesManager() {
	vars.resize(DEFAULT_BUFFER_SIZE);
}

void VariablesManager::ensure_buffer_size_for_new_var() {
    size_t buf_size = vars.size();
    if (buf_size == vars_count + 1) {
        vars.resize(buf_size * 2);
    }
}

var_id_t VariablesManager::reset(const wchar_t* name, const wchar_t* default_value) {
	ensure_buffer_size_for_new_var();

	var_id_t id = vars_count++;
	Variable& new_var = vars[id];

	new_var.name = name;
	new_var.markup.clear();
	new_var.markup << default_value;

	return id;
}
