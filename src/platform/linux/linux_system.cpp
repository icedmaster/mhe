#include "platform/linux/linux_system.hpp"
#include <sys/time.h>


namespace mhe {
namespace linuxsys {

namespace 
{
timespec start;
}


void start_platform()
{
	clock_gettime(CLOCK_MONOTONIC, &start);
}

void stop_platform()
{}

cmn::uint get_current_tick()
{
	timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (now.tv_sec - start.tv_sec) * 1000 + (now.tv_nsec - start.tv_nsec) / 1000000;
}

}}
