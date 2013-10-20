//
//	Different utilities for logging system
//
#ifndef _LOGUTILS_HPP_
#define _LOGUTILS_HPP_

#include "global_log.hpp"

namespace mhe {
namespace utils {

inline void create_standart_log()
{
	GlobalLog::instance().add(new StdLog);
	GlobalLog::instance().add(new FileLog("log.txt"));
}

}}

#endif
