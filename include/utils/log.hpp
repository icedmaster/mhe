//
//	Logging system base class
//
#ifndef _LOG_HPP_
#define _LOG_HPP_

#include "types.hpp"
#include <string>
#include <cstdarg>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>

namespace mhe
{
	enum dbg_level
	{
		level_all,
		level_dbg,
		level_info,
		level_warn,
		level_err,
		level_none
	};

	class log
	{
		private:
			bool is_open_;
			
			std::string get_date_time()
			{
				using namespace boost::gregorian;
				using namespace boost::posix_time;
				ptime now = second_clock::local_time();
				return to_simple_string(now);
			}

			void write2log(const std::string& str, bool use_date_time)
        	{
				std::string s = (use_date_time) ? "[" : "";
				if (use_date_time)
				{
					s += get_date_time();
					s += "] ";
				}
				s += str;
				if (use_date_time)
					s += "\n";
            	write_impl(s);
			}

			void write_p(char *buf, int size, bool use_date_time)
			{
				write2log(std::string(buf), use_date_time);
			}
			
			void write_p(const std::string& s, bool use_date_time)
			{
            	write2log(s, use_date_time);
			}
			
			void write_p(const std::wstring& ws, bool use_date_time)
			{
				//write_date_time();
				//write_impl(ws);
			}

			void close_p()
        	{
                close_impl();
            }

			bool check_dbg_level(dbg_level module_lvl, dbg_level msg_lvl)
        	{
                return (module_lvl < msg_lvl) ? true : false;
            }

		protected:
			virtual void write_impl(const std::string& s) = 0;
			
			virtual void write_impl(const std::wstring& ws)
			{
			}
			
			virtual void close_impl() = 0;

			void set_open()
        	{
                is_open_ = true;
			}

			void set_close()
        	{
                is_open_ = false;
            }
		public:
			log() :
				is_open_(false)
			{}
			
			virtual ~log()
			{				
			}
			
			void write(const std::string& s, bool use_date_time = true)
			{
				if (!is_open_) return;
				write_p(s, use_date_time);
			}
			
			void write(const std::wstring& ws, bool use_date_time = true)
			{
				if (!is_open_) return;
				write_p(ws, use_date_time);
			}

			void printf(const char* fmt, ...)
			{
				if (!is_open_) return;
				char buf[1024];
				va_list vaList;
				va_start(vaList, fmt);
				int len = vsnprintf(buf, sizeof(buf), fmt, vaList);
				va_end(vaList);
				write_p(buf, len, true);
			}

			void dbg_write(dbg_level module_lvl, dbg_level msg_lvl,
						   const std::string& s)
			{
				if (!check_dbg_level(module_lvl, msg_lvl))
					return;
            	write(s);
            }
			
			bool is_open() const
			{
				return is_open_;
			}

			void close()
        	{
            	close_p();
            }
	};
	
	
	class stdlog : public log
	{
		private:
			void close_impl()
			{}
			
			void write_impl(const std::string& s)
			{
				std::cout << s;
			}
			
			void write_impl(const std::wstring& ws)
			{}
			
		public:
			stdlog() :
				log()
			{
				set_open();
			}
			
			~stdlog()
			{
				close();
			}
	};
	

	class filelog : public log
	{
		private:
			std::ofstream of;

			void close_impl()
        	{
                of.close();
			}

			void write_impl(const std::string& s)
			{
				of << s;
			}

			void write_impl(const std::wstring& ws)
        	{
            }
		public:
			filelog() :
				log()
			{}

			~filelog()
			{
				close();
			}

			bool open(const std::string& fn)
			{
				of.open(fn.c_str());
				if (!of.is_open())
					return false;
            	set_open();
				write("Log started\n");
            	return true;
            }
    };
	
	
	class mixlog : public log
	{
		private:
			std::vector< boost::shared_ptr<log> > logs;
			
			mixlog(const mixlog&) {}
			
			void write_impl(const std::string& s)
			{
				for (cmn::uint i = 0; i < logs.size(); ++i)
					logs[i]->write(s, false);
			}
			
			void write_impl(const std::wstring& ws)
			{
				//for (cmn::uint i = 0; i < logs.size(); ++i)
				//	logs[i]->write
			}
			
			void close_impl()
			{
				for (cmn::uint i = 0; i < logs.size(); ++i)
					logs[i]->close();
			}
		public:
			mixlog() :
				log()
			{
				set_open();		// always open
			}
			
			~mixlog()
			{
				close();
			}

			void add(const boost::shared_ptr<log>& l)
			{
				if (!l) return;
				logs.push_back(l);
			}
	};
					
};

#endif
