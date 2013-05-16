#include "context.hpp"

namespace mhe {

void Context::update()
{
	driver_->clear_depth();
	driver_->clear_color();
	driver_->begin_frame();
}

void Context::flush()
{
	driver_->end_frame();
	window_system_->swap_buffers();
}

}
