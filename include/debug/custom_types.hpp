#ifndef __CUSTOM_TYPES_HPP__
#define __CUSTOM_TYPES_HPP__

#include "core/string.hpp"

namespace mhe {

enum
{
	None,
	Int,
	Float,
	String,
	Vector3,
	Vector4
};

template <class T>
struct TypeHelper { static const int type = None; };

template <>
struct TypeHelper<int> { static const int type = Int; };

template <>
struct TypeHelper<size_t> { static const int type = Int; };

template <>
struct TypeHelper<bool> { static const int type = Int; };

typedef basic_string<char, 4096> rdbg_string;

}

#endif
