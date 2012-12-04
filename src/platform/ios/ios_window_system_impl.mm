#include "platform/ios/ios_window_system_impl.hpp"

#include "platform/ios/mhe_glview.h"
#include "utils/global_log.hpp"

namespace mhe {
namespace ios {
    
iOSSurface::~iOSSurface()
{
    [view_ release];
}

bool iOSWindowSystemImpl::init(const vector2<int>& r, int /*bpp*/, bool /*fullscreen*/)
{
	context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	if (context_ == nil)
	{
		ERROR_LOG("Can't create OpenGLES 2 context");
		return false;
	}
	[EAGLContext setCurrentContext:context_];
	mheGLView* view = [[mheGLView alloc] initWithFrame:CGRectMake(0, 0, r.x(), r.y())];
	view.context = context_;
	view.drawableDepthFormat = GLKViewDrawableDepthFormat24; // TODO:
	surface_.set(view);
	return true;
}

void iOSWindowSystemImpl::close()
{
	[context_ release];
}
    
void iOSWindowSystemImpl::swap_buffers()
{
}

}}
