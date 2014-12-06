#include "platform/linux/linux_system.hpp"
#include <sys/time.h>


namespace mhe {
namespace linuxsys {

namespace 
{
timespec start;
float delta;
float prev;
}


void start_platform()
{
	clock_gettime(CLOCK_MONOTONIC, &start);
    delta = 0.0f;
    prev = 0.0f;
}

void stop_platform()
{}

uint get_current_tick()
{
    return get_current_time() * 1000;
}

float get_current_time()
{
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1000000000.0f;
}

float get_last_delta()
{
    float result = get_current_time();
    delta = result - prev;
    prev = result;
    return delta;
}

}}
