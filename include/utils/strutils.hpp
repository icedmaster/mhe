#ifndef _STRUTILS_HPP_
#define _STRUTILS_HPP_

#define USE_BOOST_STRINGS

#ifdef USE_BOOST_STRINGS
	#include <boost/lexical_cast.hpp>
#endif

namespace mhe
{
	namespace utils
	{
		inline float to_float(const std::wstring& ws)
		{
			#ifdef USE_BOOST_STRINGS
			return boost::lexical_cast<float>(ws);
			#endif
		}

		template <class T>
		inline T to_number(const std::wstring& ws)
		{
			#ifdef USE_BOOST_STRINGS
			return boost::lexical_cast<T>(ws);
			#endif
		}

		inline std::string from_wstr(const std::wstring& ws)
		{
			// TODO:!!!!
			return std::string(ws.begin(), ws.end());						
		}
		
		inline void rtrim(std::string& s, const std::string& c = " ")
		{
			s.erase(s.find_last_not_of(c) + 1);
		}

		inline void ltrim(std::string& s, const std::string& c = " ")
		{
			s.erase(0, s.find_first_not_of(c));
		}

		inline void trim(std::string& s, const std::string& c = " ")
		{
			rtrim(s, c);
			ltrim(s, c);
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
	}
}

#endif
