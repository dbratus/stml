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

ostream* AbstractGenerator::get_output() const {
	return this->out;
}
