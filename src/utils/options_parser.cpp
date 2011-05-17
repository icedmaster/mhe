#include "utils/options_parser.hpp"

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
		// разделяем на пары
		size_t pos = 0;
		do
		{
			pos = opt.find_first_of(delimiter);
			std::string pr = opt;
			if (pos != std::string::npos)
			{
				pr = opt.substr(0, pos);
				opt.erase(0, pos + 1);
			}
			parse_pair(pr);
		}
		while (pos != std::string::npos);
	}

	void OptionsParser::parse_pair(const std::string& opt_pr)
	{
		// удалим лишние пробелы
		std::string pr = opt_pr;
		trim(pr);
		size_t pos = pr.find_first_of('=');
		if (pos == std::string::npos)
		{
			// считаем сие как bool значение
			m[pr] = "t";
			return;
		}

		std::string first = pr.substr(0, pos);
		trim(first);
		std::string second = pr.substr(pos + 1, pr.length() - pos - 1);
		trim(second);
		m[first] = second;
	}

	bool OptionsParser::getString(const std::string& name, std::string& value, const std::string& def_value) const
	{
		return get_option(name, value, def_value);
	}

	bool OptionsParser::getInteger(const std::string& name, int& value, int def_value) const
	{
		return get_option(name, value, def_value);
	}

	bool OptionsParser::getBool(const std::string& name, bool& value, bool def_value) const
	{
		return get_option(name, value, def_value);
	}

}
}
