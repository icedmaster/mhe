#include "platform/qt/qt_window_system.hpp"

namespace mhe {
namespace qt {

QtWindowSystem::QtWindowSystem() :
	view_(nullptr)
{}

bool QtWindowSystem::init(const vector2<int>& r, int /*bpp*/, bool /*fullscreen*/)
{
	if (view_ == nullptr)
	{
		// check for Qt Application class instance
		if (QApplication::instance() == nullptr)
		{
			int argc = 0;
			static QApplication application(argc, nullptr);
		}
		view_ = new QtView;
		view_->show();
		view_->setGeometry(0, 0, r.x(), r.y());
	}

	return true;
}

void QtWindowSystem::close()
{
}

void QtWindowSystem::set_caption(const std::string& /*caption*/)
{
}

void QtWindowSystem::swap_buffers()
{}

}}
