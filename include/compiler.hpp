#ifndef __COMPILER_HPP__
#define __COMPILER_HPP__

#ifdef _MSC_VER
#define MHE_VS
#endif

#ifdef MHE_VS
#define FUNCTION_DESCRIPTION_MACRO						__FUNCSIG__
#else
#define FUNCTION_DESCRIPTION_MACRO						__PRETTY_FUNCTION__
#endif

#endif
