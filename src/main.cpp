#include "test_config.hpp"

#ifdef ARGPARSER_TEST
    #include "argparser_test.hpp"
#endif

#ifdef LOG_TEST
    #include "log_test.hpp"
#endif

#ifdef MISC_TEST
    #include "misc_test.hpp"
#endif

int main(int argc, char **argv)
{
    #ifdef ARGPARSER_TEST
    return argparser_test(argc, argv);
    #endif

    #ifdef LOG_TEST
    return log_test(argc, argv);
    #endif

    #ifdef MISC_TEST
    return misc_test(argc, argv);
    #endif

    return 0;
}
