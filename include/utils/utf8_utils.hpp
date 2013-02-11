#ifndef __UTF8_UTILS_HPP__
#define __UTF8_UTILS_HPP__

#include <string>
#include <stdint.h>

namespace mhe {

typedef uint32_t utf32_char;
typedef std::basic_string<utf32_char> utf32_string;

namespace utils
{
utf32_string utf8_to_utf32(const std::string& from);
}

}

#endif
