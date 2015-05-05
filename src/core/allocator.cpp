#include "core/allocator.hpp"

namespace mhe {

namespace {
allocator* global_allocator = nullptr;
}

void set_default_allocator(allocator* alloc)
{
	global_allocator = alloc;
}

allocator* default_allocator()
{
	return global_allocator;
}

void create_default_allocator()
{
	global_allocator = new sysallocator;
}

void destroy_default_allocator()
{
	delete global_allocator;
}

}
