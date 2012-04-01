#ifndef __PLATFORM_SYSUTILS_HPP__
#define __PLATFORM_SYSUTILS_HPP__

#include "config.hpp"

#ifdef MHE_LINUX
#include "linux/linux_system.hpp"
#endif

#ifdef MHE_WIN
#include "win/win_system.hpp"
#endif

namespace mhe {
namespace impl {

inline void start_platform()
{
#ifdef MHE_LINUX
	linuxsys::start_platform();
#endif

#ifdef MHE_WIN
	winsys::start_platform();
#endif
}

inline cmn::uint get_current_tick()
{
#ifdef MHE_LINUX
	return linuxsys::get_current_tick();
#endif

#ifdef MHE_WIN
	return winsys::get_current_tick();
#endif
}

}}

#endif
