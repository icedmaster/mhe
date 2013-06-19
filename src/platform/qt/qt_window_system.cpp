#include "platform/qt/qt_window_system.hpp"

#include "utils/global_log.hpp"

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
		view_->setGeometry(0, 0, r.x(), r.y());
		set_format(WindowContextFormat());
	}

	return true;
}

bool QtWindowSystem::set_format(const WindowContextFormat& format)
{
	INFO_LOG("QtWindowSystem::set_format. Try to set OpenGL version:" << format.major_version << "." << format.minor_version);
	QGLFormat qt_format = view_->format();
	qt_format.setVersion(format.major_version, format.minor_version);
	QRect rect = view_->geometry();
	if ( (view_ != nullptr ) && view_->isVisible() )
	{
		delete view_;
		view_ = new QtView;
	}
	view_->setGeometry(rect);	
	view_->makeCurrent();
	view_->setFormat(qt_format);
	view_->show();
	int major = view_->format().majorVersion();
	int minor = view_->format().minorVersion();
	INFO_LOG("QtWindowSystem::set_format. Use OpenGL version:" << major << "." << minor);
	INFO_LOG("QtWindowSystem::set_format. Available OpenGL versions:" << QGLFormat::openGLVersionFlags());
	return ((format.major_version == static_cast<cmn::uint>(major)) &&
			(format.minor_version == static_cast<cmn::uint>(minor)));
}

void QtWindowSystem::close()
{
}

void QtWindowSystem::set_caption(const std::string& /*caption*/)
{
}

void QtWindowSystem::swap_buffers()
{
}

}}
