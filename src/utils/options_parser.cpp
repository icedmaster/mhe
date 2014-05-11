#include "utils/options_parser.hpp"

#include "utils/strutils.hpp"

namespace mhe {
namespace utils {

OptionsParser::OptionsParser(const std::string& options, char delimiter)
{
	parse(options, delimiter);
}

void OptionsParser::process(const std::string& opt, char delimiter)
{
	m.clear();
	parse(opt, delimiter);
}

void OptionsParser::parse(const std::string& options, char delimiter)
{
	std::string opt = replace(options, '\r');
	const std::vector<std::string> pairs = split(opt, &delimiter);
	for (size_t i = 0; i < pairs.size(); ++i)
		parse_optpair(pairs[i]);
}

void OptionsParser::parse_optpair(const std::string& opt_pr)
{
	const std::pair<std::string, std::string>& pair = parse_pair(opt_pr);
	m[pair.first] = (pair.second.empty()) ? "t" : pair.second;
}

bool OptionsParser::get_string(const std::string& name, std::string& value, const std::string& def_value) const
{
	return get_option(name, value, def_value);
}

bool OptionsParser::get_integer(const std::string& name, int& value, int def_value) const
{
	return get_option(name, value, def_value);
}

bool OptionsParser::get_bool(const std::string& name, bool& value, bool def_value) const
{
	return get_option(name, value, def_value);
}

std::pair<std::string, std::string> OptionsParser::parse_pair(const std::string& str)
{
	std::string key, value;
	const std::vector<std::string>& res = split(str, "=");
	if (res.size() < 2)
		key = trim_copy(str);
	else
	{
		key = trim_copy(res[0]);
		value = trim_copy(res[1]);
	}
	return std::make_pair(key, value);
}

}
}
