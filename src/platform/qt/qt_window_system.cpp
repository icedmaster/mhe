#include "platform/qt/qt_window_system.hpp"

namespace mhe {
namespace qt {

bool QtWindowSystem::init(const vector2<int>& r, int /*bpp*/, bool /*fullscreen*/)
{
	if (view_ == nullptr)
		view_ = new QtView;
	view_->setGeometry(0, 0, r.x(), r.y());
}

void QtWindowSystem::close()
{
}

void QtWindowSystem::set_caption(const std::string& caption)
{
}

void QtWindowSystem::swap_buffers()
{}

}}
