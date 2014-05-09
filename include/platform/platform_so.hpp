#ifndef __PLATFORM_SO_HPP__
#define __PLATFORM_SO_HPP__

#include "core/config.hpp"
#include "core/types.hpp"

#ifdef MHE_WIN
#include "win/win_so.hpp"
#endif

#ifdef MHE_LINUX
#include "linux/linux_so.hpp"
#endif

#ifdef MHE_MACOS
#include "macos/macos_so.hpp"
#endif

namespace mhe {
namespace impl {

#ifdef MHE_WIN
typedef winsys::so_handle so_handle;
#endif

#ifdef MHE_LINUX
typedef linuxsys::so_handle so_handle;
#endif

#ifdef MHE_MACOS
typedef macossys::so_handle so_handle;
#endif

#ifdef MHE_IOS
typedef void* so_handle;
#endif

inline so_handle open_so(const std::string& name)
{
#ifdef MHE_WIN
	return winsys::open_so(name);
#endif

#ifdef MHE_LINUX
	return linuxsys::open_so(name);
#endif

#ifdef MHE_MACOS
	return macossys::open_so(name);
#endif
    (void)name;
	return nullptr;
}

inline void close_so(so_handle handle)
{
#ifdef MHE_WIN
	winsys::close_so(handle);
#endif

#ifdef MHE_LINUX
	linuxsys::close_so(handle);
#endif

#ifdef MHE_MACOS
	macossys::close_so(handle);
#endif
    (void)handle;
}

inline void* get_proc_addr(so_handle handle, const std::string& name)
{
#ifdef MHE_WIN
	return winsys::get_proc_addr(handle, name);
#endif

#ifdef MHE_LINUX
	return linuxsys::get_proc_addr(handle, name);
#endif

#ifdef MHE_MACOS
	return macossys::get_proc_addr(handle, name);
#endif
    (void)name; (void)handle;
	return nullptr;
}

#ifdef MHE_OPENGL
inline bool init_opengl_so()
{
#ifdef MHE_WIN
	return winsys::init_opengl_so();
#endif

#ifdef MHE_LINUX
	return linuxsys::init_opengl_so();
#endif

#ifdef MHE_MACOS
	return macossys::init_opengl_so();
#endif
}

inline void deinit_opengl_so()
{
#ifdef MHE_WIN
	winsys::deinit_opengl_so();
#endif

#ifdef MHE_LINUX
	linuxsys::deinit_opengl_so();
#endif

#ifdef MHE_MACOS
	macossys::deinit_opengl_so();
#endif
}

inline void* get_opengl_proc_addr(const std::string& name)
{
#ifdef MHE_WIN
	return winsys::get_opengl_proc_addr(name);
#endif
#ifdef MHE_LINUX
	return linuxsys::get_opengl_proc_addr(name);
#endif

#ifdef MHE_MACOS
	return macossys::get_opengl_proc_addr(name);
#endif    
	return nullptr;
}
#endif	// MHE_OPENGL

}}

#endif
