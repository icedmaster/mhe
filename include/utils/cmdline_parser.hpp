#ifndef __CMDLINE_PARSER_HPP__
#define __CMDLINE_PARSER_HPP__

#include "core/keyvalue.hpp"
#include "core/string.hpp"

namespace mhe {
namespace utils {

class MHE_EXPORT CmdLineParser
{
public:
    CmdLineParser(int argc, char** argv)
    {
        parse(argc, argv);
    }

    template <class T>
    T get(const char* key, const char* alt) const
    {
        if (keyvalue_.has(string(key)))
            return keyvalue_.get<T>(string(key));
        return keyvalue_.get<T>(alt);
    }
    
    template <class T>
    T get(const char* key, const char* alt, const T& default_value) const
    {
        if (!keyvalue_.has(string(key)) && !keyvalue_.has(string(alt)))
            return default_value;
        return get<T>(key, alt);
    }
private:
    void parse(int argc, char** argv);

    KeyValue<string, string> keyvalue_;
};

}}

#endif
