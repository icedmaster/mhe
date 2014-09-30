#ifndef __WIN_SO_HPP__
#define __WIN_SO_HPP__

#include <windows.h>
#include <string>
#include "core/config.hpp"

namespace mhe {
namespace winsys {

typedef HMODULE so_handle;

so_handle open_so(const std::string& name);
void close_so(so_handle handle);
void* get_proc_addr(so_handle handle, const std::string& name);

#ifdef MHE_OPENGL
bool init_opengl_so();
void deinit_opengl_so();
void* get_opengl_proc_addr(const std::string& name);
#endif

}}

#endif