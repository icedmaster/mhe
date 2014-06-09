#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#include "core/ref_ptr.hpp"

namespace mhe {
namespace utils {

class MixLog;

inline std::string string_date()
{
	time_t t = time(0);
	char buf[100];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
	return std::string(buf);
}

class Log : public ref_counter
{
public:
	enum
	{
		all,
		debug,
		info,
		warning,
		error,
		none
	};

	Log() : log_level_(all), use_datetime_(true) {}
	virtual ~Log() {}

	void set_level(int level)
	{
		log_level_ = level;
	}

	void use_datetime(bool use)
	{
		use_datetime_ = use;
	}

	void write(int level, const std::string& str)
	{
		if (level >= log_level_)
		{
			std::string s = (use_datetime_) ? ("[" + string_date() + "] " + str) : str;
			s = "(" + get_string_level(level) + ")" + s;
			s += "\n";
			write_impl(s);
		}
	}

	void write(int level, const std::stringstream& ss)
	{
		write(level, ss.str());
	}
protected:
	std::string get_string_level(int level) const
	{
		switch (level)
		{
			case all: return "ALL";
			case debug: return "DEBUG";
			case info: return "INFO";
			case warning: return "WARNING";
			case error: return "ERROR";
		}
		return "";
	}
private:
	friend class MixLog; 	// MixLog need access to write_impl() method
	virtual void write_impl(const std::string& str) = 0;
private:
	int log_level_;
	bool use_datetime_;
};

class StdLog : public Log
{
public:
	StdLog() : Log() {}
private:
	void write_impl(const std::string& str)
	{
		std::cout << str;
	}
};

class FileLog : public Log
{
public:
	FileLog(const std::string& filename) :
		Log(),
		of(filename.c_str())
	{}

	~FileLog()
	{
		of.close();
	}
private:
	void write_impl(const std::string& str)
	{
		assert(of.is_open());
		of << str;
	}

	std::ofstream of;
};


class MixLog : public Log
{
public:
	MixLog() : Log() {}
	void add(Log* log)
	{
		logs_.push_back(ref_ptr<Log>(log));
	}

	void add(const ref_ptr<Log>& log)
	{
		logs_.push_back(log);
	}
private:
	void write_impl(const std::string& str)
	{
		for (size_t i = 0; i < logs_.size(); ++i)
			logs_[i]->write_impl(str);
	}

	std::vector< ref_ptr<Log> > logs_;
};

}}	// namespaces

#endif	// __LOG_HPP__
