#include "platform/win/win_so.hpp"
#include "core/types.hpp"

#include "utils/global_log.hpp"

namespace mhe {
namespace winsys {

#ifdef MHE_OPENGL
const std::string opengl_lib_name = "opengl32.dll";
namespace
{
so_handle opengl_lib_handle = nullptr;
}
#endif

so_handle open_so(const std::string& name)
{
	return LoadLibrary(name.c_str());
}

void close_so(so_handle handle)
{
	FreeLibrary(handle);
}

void* get_proc_addr(so_handle handle, const std::string& name)
{
	return reinterpret_cast<void*>(GetProcAddress(handle, name.c_str()));
}

#ifdef MHE_OPENGL
bool init_opengl_so()
{
	//opengl_lib_handle = open_so(opengl_lib_name);
	//return (opengl_lib_handle != nullptr);
	return true;
}

void deinit_opengl_so()
{
	//close_so(opengl_lib_handle);
}

void* get_opengl_proc_addr(const std::string& name)
{	
	void* result = wglGetProcAddress(name.c_str());	
	if (result == 0)
	{
		WARN_LOG("windows::get_opengl_proc_addr can not load method " << name << " error:" << GetLastError());
	}
	return result;
}
#endif

}}
