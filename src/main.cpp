#include "test_config.hpp"

#ifdef ARGPARSER_TEST
    #include "argparser_test.hpp"
#endif

int main(int argc, char **argv)
{
    #ifdef ARGPARSER_TEST
    return argparser_test(argc, argv);
    #endif

    return 0;
}
