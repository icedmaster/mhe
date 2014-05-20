#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "window_system.hpp"
#include "video_driver.hpp"
#include "core/pool.hpp"

namespace mhe {

struct Context
{
	WindowSystem window_system;
	Driver driver;
};

}

#endif
