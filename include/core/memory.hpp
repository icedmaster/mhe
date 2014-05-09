#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include "types.hpp"

namespace mhe {
namespace details {

void* allocate(size_t size);
void free(void* p);

}

uint64_t allocated_memory_size();
uint64_t total_allocated_size();
uint64_t total_freed_size();
uint64_t allocations_count();
uint64_t frees_count();

void print_memory_info();

}

inline void* operator new(size_t size)
{
	return mhe::details::allocate(size);
}

inline void operator delete(void* p)
{
	return mhe::details::free(p);
}

#endif
