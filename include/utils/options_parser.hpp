#ifndef _OPRIONS_PARSER_HPP_
#define _OPTIONS_PARSER_HPP_

#include "strutils.hpp"
#include <map>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <cctype>

namespace mhe {
namespace utils {

template<class T>
inline bool cast_value(T& val, const std::string& valbase)
{
	try
	{
		val = boost::lexical_cast<T>(valbase);
	}
	catch (std::exception&)
	{
		return false;
	}
	return true;
}

template<>
inline bool cast_value(std::string& val, const std::string& valbase)
{
	val = valbase;
	return true;
}

template<>
inline bool cast_value(bool& val, const std::string& valbase)
{
	const std::string true_values[4] = {"1", "t", "true", "yes"};
	val = true;
			
	std::string lval = valbase;
	std::transform(valbase.begin(), valbase.end(), lval.begin(), ::tolower);
	if (std::find(true_values, true_values + 4, lval) == (true_values + 4) )
		val = false;
	return true;
}

class OptionsParser
{
public:
	OptionsParser(const std::string& opt, char delimiter = ';');

	void process(const std::string& opt, char delimiter = ';');

	bool get_string(const std::string& name, std::string& value, const std::string& def_value) const;
	bool get_integer(const std::string& name, int& value, int def_value) const;
	bool get_bool(const std::string& name, bool& value, bool def_value) const;
	bool get_float(const std::string& name, float& value, float def_value) const;
	template <class Type>
	bool get(const std::string& name, Type& value, Type def_value) const
	{
		return get_option(name, value, def_value);
	}

	template <class Type>
	Type get(const std::string& name) const
	{
		Type t;
		get_option(name, t, Type());
		return t;
	}

	static std::pair<std::string, std::string> parse_pair(const std::string& str);
private:
	void parse(const std::string& options, char delimiter);
	void parse_optpair(const std::string& pr);

	typedef std::map<std::string, std::string> optmap;
	optmap m;

	template <class T>
	bool get_option(const std::string& name, T& val, T def_val) const
	{
		optmap::const_iterator it = m.find(name);
		if ( (it == m.end()) || !cast_value(val, it->second))
		{
			val = def_val;
			return false;
		}
		return true;
	}
};

}}

#endif

