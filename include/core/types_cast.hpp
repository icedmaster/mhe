#ifndef __TYPES_CAST_HPP__
#define __TYPES_CAST_HPP__

#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "assert.hpp"
#include "types.hpp"

namespace mhe {

namespace details
{
template <class T>
inline std::string to_string_types_cast(const T& value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
}

template <class U, class T>
inline U types_cast(const T& value)
{
    return static_cast<U>(value);
}

template <>
inline std::string types_cast(const int& value)
{
	return details::to_string_types_cast(value);
}
    
template <>
inline std::string types_cast(const unsigned int& value)
{
    return types_cast<std::string>(static_cast<const int&>(value));
}

template <>
inline std::string types_cast(const unsigned long& value)
{
	return details::to_string_types_cast(value);
}

template<>
inline std::string types_cast(const float& value)
{
	return details::to_string_types_cast(value);
}

template <>
inline std::string types_cast(const bool& value)
{
	return details::to_string_types_cast(static_cast<size_t>(value));
}

template <>
inline const char* types_cast(const int& value)
{
    return types_cast<std::string>(value).c_str();
}
    
template <>
inline const char* types_cast(const unsigned int& value)
{
    return types_cast<std::string>(value).c_str();
}
    
template <>
inline int types_cast(const std::string& value)
{
    return atoi(value.c_str());
}

template <>
inline size_t types_cast(const std::string& value)
{
	return atoi(value.c_str());
}

template <>
inline bool types_cast(const std::string& value)
{
	return types_cast<size_t>(value) > 0;
}

template <>
inline float types_cast(const std::string& value)
{
	return static_cast<float>(atof(value.c_str()));
}

template <class U, class V>
U checked_static_cast(V value)
{
	ASSERT(dynamic_cast<U>(value) != nullptr, "Invalid cast");
	return static_cast<U>(value);
}

}

#endif
