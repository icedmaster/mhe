#include "utils/cmdline_parser.hpp"

#include "utils/global_log.hpp"

namespace mhe {
namespace utils {

void CmdLineParser::parse(int argc, char** argv)
{
    for (int i = 0; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            string key = string(argv[i]);
            string value = "1";
            if (i < argc - 1 && argv[i + 1][0] != '-')
                value = string(argv[++i]);
            keyvalue_.add(key, value);
        }
        else
        {
        }
    }
}

}}
