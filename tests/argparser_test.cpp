#include <iostream>
#include "argparser_test.hpp"
#include "utils/argparser.hpp"

int argparser_test(int argc, char **argv)
{
    mhe::arg_parser ap(argc, argv);
    ap.add_arg("-s", mhe::arg_parser::str_type);

    if (!ap.process())
    {
        std::cout << "process() failed\n";
        return 1;
    }

    std::string s;
    if (!ap.get("-s", s))
        std::cout << "get() failed\n";
    std::cout << "s=" << s << std::endl;

    return 0;
}
