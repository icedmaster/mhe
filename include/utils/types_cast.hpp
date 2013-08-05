#ifndef __TYPES_CAST_HPP__
#define __TYPES_CAST_HPP__

#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

namespace mhe {
namespace utils {

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
inline float types_cast(const std::string& value)
{
	return atof(value.c_str());
}

}}

#endif
