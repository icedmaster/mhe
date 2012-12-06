#include "context.hpp"

namespace mhe {

void Context::update()
{
	driver_->clear_depth();
	driver_->clear_color();

	driver_->begin_render();
}

void Context::flush()
{
	driver_->end_render();
	window_system_->swap_buffers();
}

}
