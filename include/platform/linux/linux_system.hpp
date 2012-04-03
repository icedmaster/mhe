#ifndef __LINUX_SYSTEM_HPP__
#define __LINUX_SYSTEM_HPP__

#include "types.hpp"

namespace mhe {
namespace linuxsys {

void start_platform();
void stop_platform();
cmn::uint get_current_tick();

}}

#endif
