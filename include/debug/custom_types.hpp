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
	static void serialize(char* buffer, size_t /*buffer_size*/, int value)
	{
		::memcpy(buffer, &value, sizeof(uint32_t));
	}

	static int deserialize(const char* buffer)
	{
		return *(reinterpret_cast<const int32_t*>(buffer));
	}
};

#ifndef MHE_VS
template <>
struct TypeHelper<unsigned int>
{
	static const int type = Int;
	static void serialize(char* buffer, size_t /*buffer_size*/, int value)
	{
		::memcpy(buffer, &value, sizeof(uint32_t));
	}

	static int deserialize(const char* buffer)
	{
		return *(reinterpret_cast<const int32_t*>(buffer));
	}
};
#endif

template <>
struct TypeHelper<size_t>
{
	static const int type = Int;
	static void serialize(char* buffer, size_t /*buffer_size*/, int value)
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

inline uint32_t construct(const char* data)
{
	return ((data[0] << 24) & 0xff000000) | ((data[1] << 16) & 0xff0000) | ((data[2] << 8) & 0xff00) | (data[3] & 0xff);
}

inline void split(uint32_t value, char* buffer)
{
	buffer[0] = (value >> 24) & 0xff;
	buffer[1] = (value >> 16) & 0xff;
	buffer[2] = (value >> 8) & 0xff;
	buffer[3] = value & 0xff;
}

inline size_t serialize(char* dst, int type, const char* src)
{
	switch (type)
	{
	case Int:
	{
		uint32_t n = *(reinterpret_cast<const uint32_t*>(src));
		split(n, dst);
		return sizeof(int32_t);
	}
	default: return 0;
	}
	return 0;
}

}

#endif
