#include "argparser.hpp"
#include <iostream>
#include <stdlib.h>

using namespace mhe;

const std::string true_values[]  = {"true", "TRUE", "t", "1"};
const std::string false_values[] = {"false", "FALSE", "f", "0"};
const int bool_values_variants_number = 4;

bool arg_parser::find_arg(const std::string& name) const
{
	for (cmn::uint i = 0; i < arguments.size(); ++i)
	{
		if ( (arguments[i].name == name) || (arguments[i].long_name == name) )
			return true;
	}

	return false;
}


arg_parser::argument& arg_parser::get_arg(const std::string& name)
{
	for (cmn::uint i = 0; i < arguments.size(); ++i)
	{
		if ( (arguments[i].name == name) || (arguments[i].long_name == name) )
			return arguments[i];
	}

	throw mhe::exception("Argument not found");
}


const arg_parser::argument& arg_parser::get_arg(const std::string& name) const
{
	for (cmn::uint i = 0; i < arguments.size(); ++i)
	{
		if ( (arguments[i].name == name) || (arguments[i].long_name == name) )
			return arguments[i];
	}

	throw mhe::exception("Argument not found");
}


void arg_parser::set(int argc, char **argv)
{
	argc_ = argc - 1;	// first argument is application name

	for (int i = 1; i < argc; ++i)
		argv_.push_back(std::string(argv[i]));
}


void arg_parser::add_arg(const std::string& a, arg_type type)
{
	if (find_arg(a)) return;

	argument na;
	na.name = a;
	na.type = type;

	arguments.push_back(na);
}


void arg_parser::add_longarg(const std::string& a, const std::string& la)
{
	if (!find_arg(a)) return;

	argument& arg = get_arg(a);
	arg.long_name = la;
}


bool arg_parser::is_bool_value(const std::string& s) const
{
	if (is_true_value(s) || is_false_value(s))
		return true;

	return false;
}


bool arg_parser::is_true_value(const std::string& s) const
{
	for (int i = 0; i < bool_values_variants_number; ++i)
	{
		if (s == true_values[i])
			return true;
	}
	return false;
}


bool arg_parser::is_false_value(const std::string& s) const
{
	for (int i = 0; i < bool_values_variants_number; ++i)
	{
		if (s == false_values[i])
			return true;
	}
	return false;
}


bool arg_parser::process()
{
	if (!argc_ || arguments.empty())
		return false;

	for (cmn::uint i = 0; i < argv_.size(); ++i)
	{
		std::string s = argv_[i];
		// try to get argument from string "arg=val"
		cmn::uint pos = s.find_first_of("=");
		if (pos != std::string::npos)
		{
			if ( pos == (s.length() - 1) )	// incorrect string like "--abc="
				return false;
			// split
			std::string a = s.substr(0, pos);
			try
			{
				argument& arg = get_arg(a);
				// get value
				std::string v = s.substr(pos + 1, s.length() - pos - 1);
				arg.values.push_back(v);
			}
			catch (mhe::exception& e)
			{
				std::cout << "Unknown argument: " << a << std::endl;
				return false;
			}
		}
		else
		{
			// next string is value
			try
			{
				argument& arg = get_arg(argv_[i]);

				bool is_last_arg = false;
				if ( (i + 1) == (cmn::uint)argc_ )
					is_last_arg = true;

				if (arg.type == bool_type)
				{
					if (!is_last_arg && is_bool_value(argv_[i + 1]))
					{
						arg.values.push_back(argv_[i + 1]);
						++i;
					}
					else
						arg.values.push_back("t");
				}

				if (is_last_arg) return false;
				arg.values.push_back(argv_[++i]);
			}
			catch (mhe::exception& e)
			{
				std::cout << "Unknown argument: " << argv_[i] << std::endl;
				return false;
			}
		}
	}

	parsed = true;
	return true;
}


int arg_parser::values_count(const std::string& a) const
{
	try
	{
		return get_arg(a).values.size();
	}
	catch (mhe::exception& e)
	{
		return 0;
	}
}


bool arg_parser::get(const std::string& a, std::string& p) const
{
	try
	{
		p = get_arg(a).values.at(0);
		return true;
	}
	catch (...)	// catch also "out_of_range" exception
	{
		return false;
	}
}


bool arg_parser::get(const std::string& a, int& p) const
{
	try
	{
		p = atoi(get_arg(a).values.at(0).c_str());
		return true;
	}
	catch (...)
	{
		return false;
	}
}


bool arg_parser::get(const std::string& a, bool& p) const
{
	try
	{
		std::string s(get_arg(a).values.at(0));
		if (is_true_value(s))
			p = true;
		else if (is_false_value(s)) p = false;
		else return false;
		return true;
	}
	catch (...)
	{
		return false;
	}
}


bool arg_parser::get(const std::string& a, std::vector<int>& pv) const
{
	try
	{
		argument arg = get_arg(a);

		for (cmn::uint i = 0; i < arg.values.size(); ++i)
			pv.push_back(atoi(arg.values[i].c_str()));
		return true;
	}
	catch (mhe::exception& e)
	{
		return false;
	}
}


bool arg_parser::get(const std::string& a, std::vector<std::string>& pv) const
{
	try
	{
		argument arg = get_arg(a);
		pv.assign(arg.values.begin(), arg.values.end());
		return true;
	}
	catch (mhe::exception& e)
	{
		return false;
	}
}
