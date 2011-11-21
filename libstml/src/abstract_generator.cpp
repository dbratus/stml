#include "../include/stml.hpp"
#include "../include/stml_exception.hpp"
#include "../include/abstract_generator.hpp"

#include "../include/generators/html_generator.hpp"
#include "../include/generators/tex_generator.hpp"

#include <memory.h>

using namespace stml;
using namespace std;

AbstractGenerator* stml::create_generator(GeneratorTypes generator_type) {
	switch (generator_type) {
	case GENERATOR_HTML:
		return new HtmlGenerator();
	case GENERATOR_TEX:
		return new TexGenerator();
	default:
		throw StmlException(StmlException::UNSUPPORTED_GENERATOR_TYPE);
	}
}

AbstractGenerator::~AbstractGenerator() {
}

void AbstractGenerator::set_output(ostream* out) {
    this->out = out;
}

void AbstractGenerator::Parameter::append_markup_to_value(
		const MarkupBuilder& markup) {
	if (!value.empty()) {
		value += '\n';
	}

	markup.append(value);
}

const string& AbstractGenerator::Parameter::as_string() const {
	return value;
}

bool AbstractGenerator::Parameter::as_boolean() const {
	return value != PARAM_FALSE_VALUE;
}
