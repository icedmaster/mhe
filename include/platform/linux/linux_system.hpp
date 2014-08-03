#ifndef __LINUX_SYSTEM_HPP__
#define __LINUX_SYSTEM_HPP__

#include "core/types.hpp"

namespace mhe {
namespace linuxsys {

void start_platform();
void stop_platform();
mhe::uint get_current_tick();
float get_current_time();
float get_last_delta();

}}

#endif
