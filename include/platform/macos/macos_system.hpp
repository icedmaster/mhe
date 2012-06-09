#ifndef __MACOS_SYSTEM_HPP__
#define __MACOS_SYSTEM_HPP__

#include "types.hpp"

namespace mhe {
namespace macossys {

void start_platform();
void stop_platform();
cmn::uint get_current_tick();

}}

#endif
