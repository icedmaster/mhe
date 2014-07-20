#ifndef __HASH_HPP__
#define __HASH_HPP__

#include "types.hpp"

namespace mhe {

typedef uint32_t hash_type;

template <class T>
inline hash_type hash(const T& value)
{
	return static_cast<hash_type>(value);
}

inline hash_type hash(const char* value)
{
	hash_type h = 0;
	for (; *value; ++value)
		h = h * 5 + *value;
	return h;
}

template <>
inline hash_type hash(const std::string& value)
{
	return hash(value.c_str());
}

}

#endif
