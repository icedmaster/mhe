#ifndef __GLOBAL_LOG_HPP__
#define __GLOBAL_LOG_HPP__

#include "log.hpp"

namespace mhe {
namespace utils {

class GlobalLog : public MixLog
{
public:
	static GlobalLog& instance()
	{
		static GlobalLog log;
		return log;
	}
private:
	GlobalLog() {}
	~GlobalLog() {}
};

}}

#define GLOBAL_LOG(level, arg)										\
{																	\
	std::stringstream ss;											\
	ss << arg;														\
	mhe::utils::GlobalLog::instance().write(level, ss);				\
}

#define DEBUG_LOG(arg)														\
{																			\
	std::stringstream ss;													\
	ss << arg;																\
	mhe::utils::GlobalLog::instance().write(mhe::utils::Log::debug, ss);	\
}

#define INFO_LOG(arg)														\
{																			\
	std::stringstream ss;													\
	ss << arg;																\
	mhe::utils::GlobalLog::instance().write(mhe::utils::Log::info, ss);		\
}

#define WARN_LOG(arg)														\
{																			\
	std::stringstream ss;													\
	ss << arg;																\
	mhe::utils::GlobalLog::instance().write(mhe::utils::Log::warning, ss);	\
}

#define ERROR_LOG(arg)														\
{																			\
	std::stringstream ss;													\
	ss << arg;																\
	mhe::utils::GlobalLog::instance().write(mhe::utils::Log::error, ss);	\
}

#endif	// __GLOBAL_LOG_HPP__
