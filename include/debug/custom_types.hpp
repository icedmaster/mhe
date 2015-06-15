#ifndef __CUSTOM_TYPES_HPP__
#define __CUSTOM_TYPES_HPP__

#include "core/string.hpp"
#include "core/types.hpp"

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
struct TypeHelper<int>
{
	static const int type = Int;
	static void serialize(char* buffer, size_t buffer_size, int value)
	{
		::memcpy(buffer, &value, sizeof(uint32_t));
	}

	static int deserialize(const char* buffer)
	{
		return *(reinterpret_cast<const int32_t*>(buffer));
	}
};

template <>
struct TypeHelper<size_t>
{
	static const int type = Int;
	static void serialize(char* buffer, size_t buffer_size, int value)
	{
		::memcpy(buffer, &value, sizeof(int32_t));
	}

	static int deserialize(const char* buffer)
	{
		return *(reinterpret_cast<const int32_t*>(buffer));
	}
};

template <>
struct TypeHelper<bool> { static const int type = Int; };

inline size_t serialize(char* dst, int type, const char* src)
{
	switch (type)
	{
	case Int: ::memcpy(dst, src, sizeof(int32_t)); return sizeof(int32_t);
	default: return 0;
	}
	return 0;
}

}

#endif
