#ifndef __ASSERT_HPP__
#define __ASSERT_HPP__

#include <cassert>
#include <iostream>
#include "compiler.hpp"

#ifndef MHE_DEPLOY
#define ASSERT(condition, text)                     \
{															  \
	if (!(condition)) {										  \
	    std::cerr << FUNCTION_DESCRIPTION_MACRO << " " << text << "\n";	\
		assert(0); }                                          \
}
#define NOT_IMPLEMENTED_ASSERT(condition, feature)	ASSERT(condition, "Feature:" << feature << " has not been implemented yet")
#else
#define ASSERT(condition, text) (void)(condition)
#define NOT_IMPLEMENTED_ASSERT(condition, feature)
#endif

#define VERIFY(condition, text) if (!condition) {ASSERT(0, text);}

#endif
