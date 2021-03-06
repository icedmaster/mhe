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
	GlobalLog::create_singleton();
	GlobalLog::instance().add(new StdLog);
	GlobalLog::instance().add(new FileLog("log.txt"));
#ifdef MHE_VS
	GlobalLog::instance().add(new VSOutputLog);
#endif
}

inline void destroy_standart_log()
{
	GlobalLog::destroy_singleton();
}

}}

#endif
