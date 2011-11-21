#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <iostream>
#include <string>

namespace stml {

class Parser {
    AbstractGeneratorPtr generator;
    AbstractParserStatePtr states[PARSER_STATES_COUNT];
    ParserStates current_state;

public:
    Parser(GeneratorTypes generator_type);

    void parse(std::istream& in, std::ostream& out);
};

}

#endif /* PARSER_HPP_ */
