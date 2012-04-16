#ifndef VARIABLES_MANAGER_HPP_
#define VARIABLES_MANAGER_HPP_

#include <string>
#include <vector>
#include "markup_builder.hpp"
#include <stdexcept>

namespace stml {

#define UNKNOWN_VAR ((var_id_t)(-1))
#define VAR_TRUE (L"y")
#define VAR_FALSE (L"n")

typedef size_t var_id_t;

/**
 * Manages creation and access to variables.
 */
class VariablesManager {
public:

	class Variable {
	public:
		std::wstring name;
		stml::MarkupBuilder markup;

		/**
		 * Default constructor.
		 */
		inline Variable() { }

		/**
		 * Copy constructor.
		 */
		inline Variable(const Variable& var) { *this = var; }

		inline Variable& operator=(const Variable& var) {
			name = var.name;
			markup = var.markup;
			return *this;
		}

		/**
		 * Returns a boolean representation of the variable.
		 */
		inline bool as_boolean() const {
			wchar_t first_char = markup.get_text()[0];
			return first_char == L'y' || first_char == L'Y';
		}

		/**
		 * Returns a string representation of the variable.
		 */
		inline const std::wstring& as_string() const {
			return markup.get_text();
		}

		/**
		 * Indicates whether the variable contains empty string.
		 */
		inline bool empty() const {
			return markup.empty();
		}
	};

private:
	static const size_t DEFAULT_BUFFER_SIZE = 20;

	std::vector<Variable> vars;
	size_t vars_count;

	/**
	 * Ensures that the variables buffer (vars) has free
	 * place for at least one variable.
	 */
	void ensure_buffer_size_for_new_var();

public:

	/**
	 * Default constructor.
	 */
	VariablesManager();

	/**
	 * Returns a writable reference to the variable with the
	 * specified id.
	 *
	 * @param	index	ID of the variable.
	 * @return	Writable reference to the variable identified by 'id'.
	 * @throws	out_of_range if the variable with the specified ID
	 *          does not exist.
	 */
	inline Variable& operator[](var_id_t id) {
		return vars[id];
	}

	/**
	 * Returns writable reference to the variable with the
	 * specified name.
	 *
	 * @param	name	Name of the variable.
	 * @return	Writable reference to the variable.
	 * @throws	out_of_range if the variable with the specified ID
	 *          does not exist.
	 */
	Variable& operator[](const std::wstring name);

	/**
	 * Creates or resets a variable. If the variable with the specified
	 * name does not exist, it is created with the specified default value.
	 * If the value already exists, its value is reset to the specified
	 * default.
	 *
	 * @param	name			name of the variable.
	 * @param	default_value	calue to be assigned to the variable.
	 * @return	id of the variable.
	 */
	var_id_t reset(const wchar_t* name, const wchar_t* default_value);

	/**
	 * Returns id of a variable by name.
	 *
	 * @param name name of a varable.
	 * @return id of the variable or UNKNOWN_VAR if such variable doesn't exist.
	 */
	var_id_t get_by_name(const wchar_t* name);
};

}

#endif /* VARIABLES_MANAGER_HPP_ */
