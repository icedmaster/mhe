#include "platform/win/win_system.hpp"
#include "platform/win/win_wrapper.hpp"

#include "core/config.hpp"
#include <mmsystem.h>

#ifdef MHE_NETWORK
#include "net/socklib.hpp"
#endif

namespace mhe {
namespace winsys {

namespace
{
DWORD start;
LARGE_INTEGER frequency;
LARGE_INTEGER hpstart;
LARGE_INTEGER prev;
float last_delta;
}

void start_platform()
{
	timeBeginPeriod(1);
	start = timeGetTime();
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&hpstart);
	QueryPerformanceCounter(&prev);

#ifdef MHE_NETWORK
	WIN_NET_START();
#endif
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
	return (now.QuadPart - hpstart.QuadPart) / static_cast<float>(frequency.QuadPart);
}

float get_last_delta()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	last_delta = (now.QuadPart - prev.QuadPart) / static_cast<float>(frequency.QuadPart);
	prev = now;
	return last_delta;
}

}}
