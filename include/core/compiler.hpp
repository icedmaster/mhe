#ifndef __COMPILER_HPP__
#define __COMPILER_HPP__

#define MHE_EXPORT

#ifdef _MSC_VER
#define MHE_VS
#undef MHE_EXPORT
#define MHE_EXPORT __declspec( dllexport )
#if _MSC_VER > 1000
#define MHE_CPP11
#define MHE_COMPILER_HAS_NULLPTR
#endif
#else
#define MHE_GCC
#endif

#ifdef MHE_VS
#define FUNCTION_DESCRIPTION_MACRO                      __FUNCSIG__
#define FUNCTION_NAME                                   __FUNCTION__
#else
#define FUNCTION_DESCRIPTION_MACRO                      __PRETTY_FUNCTION__
#define FUNCTION_NAME                                   __FUNCTION__
#endif

#ifndef NDEBUG
#define MHE_DEBUG
#else
#define MHE_RELEASE
#endif

// warnings suppressing
#ifdef MHE_GCC
#define MHE_COMPILER_ATTRIBUTE_UNUSED __attribute__((unused))
#else
#define MHE_COMPILER_ATTRIBUTE_UNUSED
#endif

#ifndef MHE_CPP11
#define override
#define final
#define static_assert(exp, text) { typedef char __LINE__##sa[exp] MHE_COMPILER_ATTRIBUTE_UNUSED; (void)text; }
#else
#endif

#ifndef MHE_COMPILER_HAS_NULLPTR
#define nullptr 0
#endif

// Disable some warnings
#ifdef MHE_VS
#pragma warning( disable: 4251 )
#pragma warning( disable: 4275 )
#define _SCL_SECURE_NO_WARNINGS
#define NOMINMAX
#endif

#endif
