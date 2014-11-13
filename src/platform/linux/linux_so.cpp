#include "platform/linux/linux_so.hpp"
#include "core/types.hpp"
#include <dlfcn.h>

namespace mhe {
namespace linuxsys {

#ifdef MHE_OPENGL
const std::string opengl_lib_name = "libGL.so";
#endif

so_handle open_so(const std::string& name)
{
	return dlopen(name.c_str(), RTLD_LAZY);
}

void close_so(so_handle handle)
{
	dlclose(handle);
}

void* get_proc_addr(so_handle handle, const std::string& name)
{
	return dlsym(handle, name.c_str());
}

#ifdef MHE_OPENGL
bool init_opengl_so()
{
	return true;
}

void deinit_opengl_so()
{
}

void* get_opengl_proc_addr(const std::string& /*name*/)
{
	return 0;
}
#endif

}}
