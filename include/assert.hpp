#ifndef __ASSERT_HPP__
#define __ASSERT_HPP__

#include <cassert>
#include "compiler.hpp"
#include "utils/global_log.hpp"

namespace mhe {

#ifdef MHE_DEBUG
#define ASSERT(condition, text)                     \
{                                                   \
	if (!(condition)) {										  \
	    ERROR_LOG(FUNCTION_DESCRIPTION_MACRO << " " << text); \
		assert(0); }                                          \
}
#else
#define ASSERT(condition, text) ;
#endif 

}

#endif
