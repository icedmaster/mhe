#ifndef __COMPILER_HPP__
#define __COMPILER_HPP__

#ifdef _MSC_VER
#define MHE_VS
#else
#define MHE_GCC
#endif

#ifdef MHE_VS
#define FUNCTION_DESCRIPTION_MACRO						__FUNCSIG__
#else
#define FUNCTION_DESCRIPTION_MACRO						__PRETTY_FUNCTION__
#endif

#ifndef NDEBUG
#define MHE_DEBUG
#else
#define MHE_RELEASE
#endif

#endif
