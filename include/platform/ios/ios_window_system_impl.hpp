// must not be directly included in C++ code!!

#ifndef __IOS_WINDOW_SYSTEM_IMPL_HPP__
#define __IOS_WINDOW_SYSTEM_IMPL_HPP__

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#include "window_system.hpp"
#include "ios_view.hpp"

namespace mhe {
namespace ios {

class iOSWindowSystemImpl
{
public:    
	bool init(const vector2<int>& r, int /*bpp*/, bool /*fullscreen*/);	
	void close();
    
    void swap_buffers();

	iOSView* view()
	{
		return &view_;
	}
private:
	EAGLContext* context_;
	iOSView view_;
};

}}

#endif
