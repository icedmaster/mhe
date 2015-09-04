#ifndef __GLOBAL_LOG_HPP__
#define __GLOBAL_LOG_HPP__

#include "log.hpp"
#include "core/singleton.hpp"

namespace mhe {
namespace utils {

class GlobalLog : public MixLog, public Singleton<GlobalLog>
{
	friend class Singleton<GlobalLog>;
private:
	GlobalLog() {}
	~GlobalLog() {}
};

}}

#define GLOBAL_LOG(level, arg)										\
{																	\
	std::stringstream __internal_global_log_ss;											\
	__internal_global_log_ss << arg;														\
	mhe::utils::GlobalLog::instance().write(level, __internal_global_log_ss);				\
}

#define DEBUG_LOG(arg)														\
{																			\
	std::stringstream __internal_global_log_ss;													\
	__internal_global_log_ss << arg;																\
	mhe::utils::GlobalLog::instance().write(mhe::utils::Log::debug, __internal_global_log_ss);	\
}

#define INFO_LOG(arg)														\
{																			\
	std::stringstream __internal_global_log_ss;													\
	__internal_global_log_ss << arg;																\
	mhe::utils::GlobalLog::instance().write(mhe::utils::Log::info, __internal_global_log_ss);		\
}

#define WARN_LOG(arg)														\
{																			\
	std::stringstream __internal_global_log_ss;													\
	__internal_global_log_ss << arg;																\
	mhe::utils::GlobalLog::instance().write(mhe::utils::Log::warning, __internal_global_log_ss);	\
}

#define ERROR_LOG(arg)														\
{																			\
	std::stringstream __internal_global_log_ss;													\
	__internal_global_log_ss << arg;																\
	mhe::utils::GlobalLog::instance().write(mhe::utils::Log::error, __internal_global_log_ss);	\
}

#endif	// __GLOBAL_LOG_HPP__
