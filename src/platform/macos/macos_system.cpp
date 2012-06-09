#include "platform/macos/macos_system.hpp"

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <time.h>

namespace mhe {
namespace macossys {

namespace {
uint64_t start_time;
}

void start_platform()
{
	start_time = mach_absolute_time();
}

void stop_platform()
{
}

cmn::uint get_current_tick()
{
	uint64_t current_time = mach_absolute_time();
	return current_time - start_time;
}

}}
