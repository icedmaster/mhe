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

template <class Str>
inline void rtrim(Str& s, const Str& c = " ")
{
	s.erase(s.find_last_not_of(c) + 1);
}

template <class Str>
inline void ltrim(Str& s, const Str& c = " ")
{
	s.erase(0, s.find_first_not_of(c));
}

template <class Str>
inline Str rtrim_copy(const Str& s, const Str& c = " ")
{
	size_t pos = s.find_last_not_of(c);
	return s.substr(0, pos + 1);
}

template <class Str>
inline Str ltrim_copy(const Str& s, const Str& c = " ")
{
	size_t pos = s.find_first_not_of(c);
	if (pos == Str::npos) return s;
	return s.substr(pos, s.length() - pos);
}

template <class Str>
inline void trim(Str& s, const Str& c = " ")
{
	rtrim(s, c);
	ltrim(s, c);
}		

template <class Str>
inline Str trim_copy(const Str& s, const Str& c = " ")
{
	if (s.empty()) return s;
	return rtrim_copy(ltrim_copy(s, c), c);
}

template <class Str>
inline Str replace(const Str& s, typename Str::value_type from, typename Str::value_type to = 0)
{
	Str out_str;
	for (size_t i = 0; i < s.size(); ++i)
	{
		typename Str::value_type c = s[i];
		if (c == from)
		{
			if (!to) continue; 
			c = to;
		}
		out_str += c;
	}
	return out_str;
}

template <class Str, class S>
inline Str replace(const Str& s, const S& from, const S& to)
{
	Str out_str;
	size_t from_len = from.length();
	for (size_t i = 0, len = s.size(); i < len;)
	{
		bool found = true;
		for (size_t j = 0; j < from_len; ++j)
		{
			if (s[i + j] != from[j])
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			out_str += to;
			i += from_len;
		}
		else
		{
			out_str += s[i];
			++i;
		}
	}
	return out_str;
}

template <class Str, class Str2>
inline Str join(const std::vector<Str>& str, const Str2& del = Str2())
{
	Str out;
	for (size_t i = 0; i < str.size(); ++i)
	{
		out += str[i];
		if (i < (str.size() - 1))
			out += del;
	}
	return out;
}

template <class Str, class Str2>
inline std::vector<Str> split(const Str& str, const Str2& del, bool keep_empty = false)
{
	std::vector<Str> splitted;
	size_t index = 0;
	while (index <= str.length())
	{
		size_t pos = str.find_first_of(del, index);
		size_t len = ((pos == Str::npos) ? str.length() : pos) - index;
		if (len)
		{
			const Str& element = str.substr(index, len);
			splitted.push_back(element);
		}
		else if (keep_empty)
			splitted.push_back(Str());
		index += (len + 1);
	}
	return splitted;
}

}}	// namespaces

#endif
