#include "platform/ios/ios_system.hpp"

#include <sys/time.h>

namespace mhe {
namespace iossys {

void start_platform()
{
}

void stop_platform()
{
}

cmn::uint get_current_tick()
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

}}
