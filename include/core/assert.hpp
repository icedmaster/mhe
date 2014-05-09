#ifndef __ASSERT_HPP__
#define __ASSERT_HPP__

#include <cassert>
#include <iostream>
#include "compiler.hpp"

#ifdef MHE_DEBUG
#define ASSERT(condition, text)                     \
{															  \
	if (!(condition)) {										  \
	    std::cerr << FUNCTION_DESCRIPTION_MACRO << " " << text << "\n";	\
		assert(0); }                                          \
}
#else
#define ASSERT(condition, text)
#endif 

#endif
