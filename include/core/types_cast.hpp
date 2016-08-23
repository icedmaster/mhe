#ifndef __TYPES_CAST_HPP__
#define __TYPES_CAST_HPP__

#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "assert.hpp"
#include "types.hpp"
#include "string.hpp"
#include "compiler.hpp"

#ifdef MHE_VS
#pragma warning( disable : 4996 )	// sprintf is unsafe function
#endif	// MHE_VS

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

inline mhe::string int_to_str(int value)
{
    char buff[64] = {};
    sprintf(buff, "%d", value);
    return mhe::string(buff);
}

inline mhe::string uint_to_str(unsigned int value)
{
    char buff[64] = {};
    sprintf(buff, "%u", value);
    return mhe::string(buff);
}

inline mhe::string float_to_str(float value)
{
    char buff[64] = {};
    sprintf(buff, "%f", value);
    return mhe::string(buff);
}

inline mhe::string ulong_to_str(unsigned long value)
{
    char buff[64] = {};
    sprintf(buff, "%lu", value);
    return mhe::string(buff);
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

template<>
inline std::string types_cast(const bool& value)
{
    return types_cast<std::string>(static_cast<unsigned int>(value));
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
inline float types_cast(const std::string& value)
{
    return static_cast<float>(atof(value.c_str()));
}

template<>
inline bool types_cast(const std::string& value)
{
    return static_cast<bool>(types_cast<size_t>(value) > 0);
}

// From mhe::string
template <>
inline int types_cast(const mhe::string& value)
{
    return atoi(value.c_str());
}

template <>
inline size_t types_cast(const mhe::string& value)
{
    return atoi(value.c_str());
}

template <>
inline float types_cast(const mhe::string& value)
{
    return static_cast<float>(atof(value.c_str()));
}

template <>
inline bool types_cast(const mhe::string& value)
{
    return static_cast<bool>(types_cast<size_t>(value) > 0);
}

template <>
inline uint8_t types_cast(const mhe::string& value)
{
    return static_cast<uint8_t>(atoi(value.c_str()));
}

template <>
inline const char* types_cast(const mhe::string& value)
{
    return value.c_str();
}

// To mhe::string
template <>
inline mhe::string types_cast(const int& value)
{
    return details::int_to_str(value);
}

template <>
inline mhe::string types_cast(const unsigned int& value)
{
    return details::uint_to_str(value);
}

template <>
inline mhe::string types_cast(const unsigned long& value)
{
    return details::ulong_to_str(value);
}

template<>
inline mhe::string types_cast(const float& value)
{
    return details::float_to_str(value);
}

template<>
inline mhe::string types_cast(const bool& value)
{
    return types_cast<std::string>(static_cast<unsigned int>(value));
}

template <class U, class V>
U checked_static_cast(V value)
{
    ASSERT(dynamic_cast<U>(value) != nullptr, "Invalid cast");
    return static_cast<U>(value);
}

}

#endif
