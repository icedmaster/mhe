// must not be directly included in C++ code!!

#ifndef __IOS_WINDOW_SYSTEM_IMPL_HPP__
#define __IOS_WINDOW_SYSTEM_IMPL_HPP__

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#include "window_system.hpp"
#import "mhe_glview.h"

namespace mhe {
namespace ios {

class iOSSurface : public Surface
{
public:
    ~iOSSurface();
    
	mheGLView* delegate() const
	{		
		return delegate_;
	}

	void set(mheGLView* delegate)
	{
		delegate_ = delegate;
	}
    
    void set_view(GLKView* view)
    {
        view_ = view;
    }
    
    GLKView* view() const
    {
        return view_;
    }
private:
	mheGLView* delegate_;
    GLKView* view_;
};

class iOSWindowSystemImpl
{
public:    
	bool init(const vector2<int>& r, int /*bpp*/, bool /*fullscreen*/);	
	void close();
    
    void swap_buffers();

	const iOSSurface* surface() const
	{
		return &surface_;
	}
private:
	EAGLContext* context_;
	iOSSurface surface_;
};

}}

#endif
