#ifndef __IOS_SYSTEM_HPP__
#define __IOS_SYSTEM_HPP__

#include "types.hpp"

namespace mhe {
namespace iossys {

void start_platform();
void stop_platform();
cmn::uint get_current_tick();

}}

#endif
