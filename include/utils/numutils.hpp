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

inline cmn::uint32_t char_to_int_le(const char* a)
{
	return ((a[3] << 24) & 0xff000000) | ((a[2] << 16) & 0xff0000) |
	((a[1] << 8) & 0xff00) | (a[0] & 0xff);
}

}}

#endif
