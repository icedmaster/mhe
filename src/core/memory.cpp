#include "core/memory.hpp"

#include "utils/global_log.hpp"

namespace mhe {

namespace {

uint64_t allocated = 0;
uint64_t total_allocated = 0;
uint64_t total_freed = 0;
uint64_t allocations = 0;
uint64_t frees = 0;

}

namespace details {

void* allocate(size_t size)
{
	size_t* p = static_cast<size_t*>(::malloc(size + sizeof(size_t)));
	if (p == nullptr)
		return 0;
	p[0] = size;
	allocated += size;
	total_allocated += size;
	++allocations;
	return reinterpret_cast<void*>(p + 1);
}

void free(void* p)
{
	if (p == nullptr) return;
	size_t* realp = static_cast<size_t*>(p);
	size_t size = realp[-1];
	::free(realp - 1);
	allocated -= size;
	total_freed += size;
	++frees;
}

} // details

uint64_t allocated_memory_size()
{
	return allocated;
}

uint64_t total_allocated_size()
{
	return total_allocated;
}

uint64_t total_freed_size()
{
	return total_freed;
}

uint64_t allocations_count()
{
	return allocations;
}

uint64_t frees_count()
{
	return frees;
}

void print_memory_info()
{
	INFO_LOG("Memory usage:\ncurrent:" << allocated << "\n" << 
			 "total allocated:" << total_allocated << "\n" <<
			 "total freed:" << total_freed << "\n" <<
			 "allocations count:" << allocations << "\n" <<
			 "frees count:" << frees);
}

}
