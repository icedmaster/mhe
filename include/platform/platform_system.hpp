#ifndef __PLATFORM_SYSTEM_HPP__
#define __PLATFORM_SYSTEM_HPP__

#include "../config.hpp"
#include "utils/logutils.hpp"

#ifdef MHE_LINUX
#include "linux/linux_system.hpp"
#endif

#ifdef MHE_WIN
#include "win/win_system.hpp"
#endif

#ifdef MHE_MACOS
#include "macos/macos_system.hpp"
#endif

#include "platform_so.hpp"

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

#ifdef MHE_MACOS
    macossys::start_platform();
#endif

#ifdef MHE_OPENGL
	if (impl::init_opengl_so())
	{
		INFO_LOG("Loaded OpenGL lib");
	}
	else 
	{
		ERROR_LOG("Can not load OpenGL lib");
	}
#endif
}

inline void stop_platform()
{
#ifdef MHE_LINUX
	linuxsys::stop_platform();
#endif

#ifdef MHE_WIN
	winsys::stop_platform();
#endif

#ifdef MHE_MACOS
    macossys::stop_platform();
#endif

#ifdef MHE_OPENGL
	deinit_opengl_so();
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

#ifdef MHE_MACOS
    return macossys::get_current_tick();
#endif
}

}}

#endif
