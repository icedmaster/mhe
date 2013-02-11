#include "utils/utf8_utils.hpp"

namespace mhe {
namespace utils {

utf32_string utf8_to_utf32(const std::string& from)
{
	const size_t length = from.length();
	utf32_string to;
	to.reserve(length);
	const char* begin = from.c_str();
	for (const char* s = begin; s < begin + length;)
	{
		char sym = *s;
		if (!(sym & 0x80))
		{
			to.push_back(sym);
			++s;
		}
		else if (!(sym & 0x20))
		{
			utf32_char c = sym & 0x1f;
			++s;
			c <<= 6;
			c += *s & 0x3f;
			to.push_back(c);
			++s;
		}
		else if (!(sym & 0x10))
		{
			utf32_char c = sym & 0xf;
			++s;
			c <<= 6;
			c += *s & 0x3f;
			++s;
			c <<= 6;
			c += *s & 0x3f;
			to.push_back(c);
			++s;
		}
	}
	return to;
}

}}
