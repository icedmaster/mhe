#ifndef _ARGPARSER_HPP_
#define _ARGPARSER_HPP_

#include <vector>
#include <string>
#include "types.hpp"

namespace mhe
{
	class arg_parser
	{
		public:
			enum arg_type
			{
				str_type,
				int_type,
				bool_type
			};
		private:
			struct argument
			{
				std::string name;
				std::string long_name;
				std::vector<std::string> values;
				arg_type type;
			};
		private:
			int argc_;
			std::vector<std::string> argv_;
			bool parsed;
			
			std::vector<argument> arguments;
			
			arg_parser(const arg_parser&) {}			
			void operator=(const arg_parser&) {}
			
			bool find_arg(const std::string& name) const;
			argument& get_arg(const std::string& name);
			const argument& get_arg(const std::string& name) const;
			bool is_bool_value(const std::string& s) const;
			bool is_true_value(const std::string& s) const;
			bool is_false_value(const std::string& s) const;
		public:
			arg_parser() :
				argc_(0),
				parsed(false)
			{}
			
			arg_parser(int argc, char **argv)
			{
				set(argc, argv);
			}
			
			bool process();
			bool is_parsed() const
			{
				return parsed;
			}
			
			void set(int argc, char **argv);
			
			void add_arg(const std::string& a, arg_type type);
			void add_longarg(const std::string& a, const std::string& la);
			
			bool get(const std::string& a, std::string& p) const;
			bool get(const std::string& a, int& p) const;
			bool get(const std::string& a, bool& p) const;
			bool get(const std::string& a, std::vector<std::string>& pv) const;
			bool get(const std::string& a, std::vector<int>& pv) const;			
			
			int values_count(const std::string& a) const;
	};
	
};

#endif
