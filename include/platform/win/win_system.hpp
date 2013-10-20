#ifndef __WIN_SYSTEM_HPP__
#define __WIN_SYSTEM_HPP__

#include "types.hpp"

namespace mhe {
namespace winsys {

void start_platform();
void stop_platform();
cmn::uint get_current_tick();

}}

#endif
