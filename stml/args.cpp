#include <cstring>
#include <unistd.h>
#include <iostream>

#include "args.hpp"

using namespace std;
using namespace stml;

Args::Args(int argc, char *argv[]) {
    char c;

    error = false;

    bool generator_specified = false;

    while ((c = getopt(argc, argv, "g:")) != -1) {
        switch (c) {
        case 'g':
            if (strcmp(optarg, "html") == 0) {
                generator_type = GENERATOR_HTML;
                generator_specified = true;
            }
            else if (strcmp(optarg, "tex") == 0) {
                generator_type = GENERATOR_TEX;
                generator_specified = true;
            }
            else {
                cerr << "Unknown generator type '" << optarg << "'." << endl;
                error = true;
            }
            break;
        case '?':
        default:
            cerr << "Unexpected option '" << optopt << "'." << endl;
            error = true;
        }
    }

    if (!generator_specified) {
        cerr << "Generator type has not been specified." << endl;
        error = true;
    }
}
