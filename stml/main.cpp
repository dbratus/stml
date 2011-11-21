#include <stml.hpp>
#include <stml_exception.hpp>

#include "error_message.hpp"
#include "args.hpp"

using namespace std;
using namespace stml;

int main(int argc, char *argv[]) {

    Args args(argc, argv);

    if (args.error) {
        return -1;
    }

	int return_code = 0;
	try {
		parse(cin, cout, args.generator_type);
	}
	catch (const StmlException& ex) {
		cerr << get_error_message(ex.get_code());

		unsigned int line_no = ex.get_line_no();
		if (line_no > 0) {
		    cerr << " at line " << line_no;
		}

		cerr << ".";

		return_code = (int)ex.get_code();
	}

	return return_code;
}
