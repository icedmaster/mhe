#include "platform/win/win_system.hpp"
#include <windows.h>
#include <mmsystem.h>

namespace mhe {
namespace winsys {

namespace
{
DWORD start;
}

void start_platform()
{
	timeBeginPeriod(1);
	start = timeGetTime();
}

void stop_platform()
{}

cmn::uint get_current_tick()
{
	const DWORD dword_max = ~0;
	DWORD now = timeGetTime();
	if (now < start)
		return (dword_max - start) + now;
	return now - start;
}

}}
