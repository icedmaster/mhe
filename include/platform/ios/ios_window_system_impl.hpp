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
    
	mheGLView* get() const
	{		
		return view_;
	}

	void set(mheGLView* view)
	{
		view_ = view;
	}
private:
	mheGLView* view_;
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
