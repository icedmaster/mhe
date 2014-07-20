#ifndef __STRUTILS_HPP__
#define __STRUTILS_HPP__

#include <vector>
#include "core/types_cast.hpp"

namespace mhe {
namespace utils {

inline float to_number(const std::string& s)
{
	return types_cast<float>(s);
}

template <class T>
inline std::string to_string(T n)
{
	return types_cast<std::string>(n);
}

inline std::string from_wstr(const std::wstring& ws)
{
	// TODO:!!!!
	return std::string(ws.begin(), ws.end());						
}

inline std::wstring to_wstring(const std::string& s)
{
	return std::wstring(s.begin(), s.end());
}
		
inline void rtrim(std::string& s, const std::string& c = " ")
{
	s.erase(s.find_last_not_of(c) + 1);
}

inline void ltrim(std::string& s, const std::string& c = " ")
{
	s.erase(0, s.find_first_not_of(c));
}

inline std::string rtrim_copy(const std::string& s, const std::string& c = " ")
{
	size_t pos = s.find_last_not_of(c);
	return s.substr(0, pos + 1);
}

inline std::string ltrim_copy(const std::string& s, const std::string& c = " ")
{
	size_t pos = s.find_first_not_of(c);
	return s.substr(pos, s.length() - pos);
}

inline void trim(std::string& s, const std::string& c = " ")
{
	rtrim(s, c);
	ltrim(s, c);
}		

inline std::string trim_copy(const std::string& s, const std::string& c = " ")
{
	if (s.empty()) return s;
	return rtrim_copy(ltrim_copy(s, c), c);
}

inline std::string replace(const std::string& s, char from, char to = 0)
{
	std::string out_str;
	for (size_t i = 0; i < s.size(); ++i)
	{
		char c = s[i];
		if (c == from)
		{
			if (!to) continue; 
			c = to;
		}
		out_str += c;
	}
	return out_str;
}

inline std::string join(const std::vector<std::string>& str, const std::string& del = std::string())
{
	std::string out;
	for (size_t i = 0; i < str.size(); ++i)
	{
		out += str[i];
		if (i < (str.size() - 1))
			out += del;
	}
	return out;
}

inline std::vector<std::string> split(const std::string& str, const std::string& del)
{
	std::vector<std::string> splitted;
	size_t index = 0;
	while (index <= str.length())
	{
		size_t pos = str.find_first_of(del, index);
		size_t len = ((pos == std::string::npos) ? str.length() : pos) - index;
		if (len)
		{
			const std::string& element = str.substr(index, len);
			splitted.push_back(element);
		}
		index += (len + 1);
	}
	return splitted;
}

}}	// namespaces

#endif
