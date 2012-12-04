#include "platform/ios/ios_window_system.hpp"

#import <GLKit/GLKit.h>
#include "utils/global_log.hpp"

namespace mhe {
namespace ios {

class iOSWindowSystemImpl
{
public:
	bool init(const vector2<int>& r, int /*bpp*/, bool /*fullscreen*/);	
	void close();

	UIView* view() const
	{
		return view_;
	}
private:
	EAGLContext* context_;
	GLKView* view_;
};

iOSWindowSystem::iOSWindowSystem() :
	impl_(new iOSWindowSystemImpl)
{}

bool iOSWindowSystem::init(const vector2<int>& r, int bpp, bool fullscreen)
{
	return impl_->init(r, bpp, fullscreen);
}

void iOSWindowSystem::close()
{
	impl_->close();
}

void iOSWindowSystem::swap_buffers()
{
}

// implementation

bool iOSWindowSystemImpl::init(const vector2<int>& r, int /*bpp*/, bool /*fullscreen*/)
{
	context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	if (context_ == nil)
	{
		ERROR_LOG("Can't create OpenGLES 2 context");
		return false;
	}
	[EAGLContext setCurrentContext:context_];
	view_ = [[GLKView alloc] initWithFrame:CGRectMake(0, 0, r.x(), r.y())];
	view_.context = context_;
	view_.drawableDepthFormat = GLKViewDrawableDepthFormat24; // TODO:
	return true;
}

void iOSWindowSystemImpl::close()
{
	[view_ release];
	[context_ release];
}

}}
