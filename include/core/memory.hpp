#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include "types.hpp"
#include "compiler.hpp"
#include "config.hpp"

namespace mhe {
namespace details {

MHE_EXPORT void* allocate(size_t size);
MHE_EXPORT void free(void* p);

}

uint64_t MHE_EXPORT allocated_memory_size();
uint64_t MHE_EXPORT total_allocated_size();
uint64_t MHE_EXPORT total_freed_size();
uint64_t MHE_EXPORT allocations_count();
uint64_t MHE_EXPORT frees_count();

MHE_EXPORT void print_memory_info();

inline void* allocate(size_t size)
{
    return mhe::details::allocate(size);
}

inline void free(void* p)
{
    mhe::details::free(p);
}

}

#ifndef MHE_SYSTEM_NEW_DELETE

void* operator new(size_t size);
void* operator new[](size_t count);
void operator delete(void* p);
void operator delete[](void* p);

#endif

#endif
