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
    size_t* p = static_cast<size_t*>(::malloc(size + 2 * sizeof(size_t)));
    if (p == nullptr)
        return 0;
    p[0] = 0xfeefabba;
    p[1] = size;
    allocated += size;
    total_allocated += size;
    ++allocations;
    p += 2;
    return reinterpret_cast<void*>(p);
}

void free(void* p)
{
    if (p == nullptr) return;
    size_t* realp = static_cast<size_t*>(p);
    if ( realp[-2] != 0xfeefabba || realp[-2] == 0xdeaddead )
        ASSERT(0, "Invalid free() call");
    size_t size = realp[-1];
    *(realp - 2) = 0xdeaddead;
    ::free(realp - 2);
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

#ifndef MHE_SYSTEM_NEW_DELETE

void* operator new(size_t size)
{
    return mhe::details::allocate(size);
}

void* operator new[](size_t count)
{
    return mhe::details::allocate(count);
}

void operator delete(void* p)
{
    return mhe::details::free(p);
}

void operator delete[](void* p)
{
    return mhe::details::free(p);
}

#endif
