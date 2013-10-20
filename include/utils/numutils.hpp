#ifndef __NUMUTILS_HPP__
#define __NUMUTILS_HPP__

#include "types.hpp"

namespace mhe {
namespace utils {

inline cmn::uint32_t char_to_int_be(const char* a)
{
	return ((a[0] << 24) & 0xff000000) | ((a[1] << 16) & 0xff0000) |
	((a[2] << 8) & 0xff00) | (a[3] & 0xff);
}

inline void int_to_char_be(uint32_t value, char* buffer)
{
	buffer[0] = (value >> 24) & 0xff; buffer[1] = (value >> 16) & 0xff;
	buffer[2] = (value >> 8) & 0xff; buffer[3] = value & 0xff;
}

inline cmn::uint32_t char_to_int_le(const char* a)
{
	return ((a[3] << 24) & 0xff000000) | ((a[2] << 16) & 0xff0000) |
	((a[1] << 8) & 0xff00) | (a[0] & 0xff);
}

inline void int_to_char_le(uint32_t value, char* buffer)
{
	buffer[3] = (value >> 24) & 0xff; buffer[2] = (value >> 16) & 0xff;
	buffer[1] = (value >> 8) & 0xff; buffer[0] = value & 0xff;
}

}}

#endif
