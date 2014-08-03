#ifndef __WIN_SYSTEM_HPP__
#define __WIN_SYSTEM_HPP__

#include "core/types.hpp"

namespace mhe {
namespace winsys {

void start_platform();
void stop_platform();
uint get_current_tick();
float get_current_time();
float get_last_delta();

}}

#endif
