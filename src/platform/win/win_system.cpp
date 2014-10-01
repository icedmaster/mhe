#include "platform/win/win_system.hpp"
#include "platform/win/win_wrapper.hpp"
#include <mmsystem.h>

namespace mhe {
namespace winsys {

namespace
{
DWORD start;
LARGE_INTEGER frequency;
LARGE_INTEGER hpstart;
float last_delta;
}

void start_platform()
{
	timeBeginPeriod(1);
	start = timeGetTime();
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&hpstart);
}

void stop_platform()
{
	timeEndPeriod(1);
}

uint get_current_tick()
{
	const DWORD dword_max = ~0;
	DWORD now = timeGetTime();
	if (now < start)
		return (dword_max - start) + now;
	return now - start;
}

float get_current_time()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	last_delta = (now.QuadPart - hpstart.QuadPart) / frequency.QuadPart;
	return now.QuadPart / frequency.QuadPart;
}

float get_last_delta()
{
	return last_delta;
}

}}
