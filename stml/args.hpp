
#ifndef ARGS_PARSER_HPP_
#define ARGS_PARSER_HPP_

#include <stml.hpp>

class Args {
public:
    Args(int argc, char *argv[]);

    stml::GeneratorTypes generator_type;
    bool error;
};

#endif /* ARGS_PARSER_HPP_ */
