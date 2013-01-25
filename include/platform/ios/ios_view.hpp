// This file must not be directly included in C++ code!!
#ifndef __IOS_VIEW_HPP__
#define __IOS_VIEW_HPP__

#include "view.hpp"
#include "mhe_glview.h"

namespace mhe {
namespace ios {

class iOSView : public View
{
public:
	iOSView() {}

	iOSView(ViewEventsHandler* events_handler) :
		View(events_handler)
	{}

	void set_glview(mheGLView* glview)
	{
		glview_ = glview;
		[glview setViewEventsHandler:events_handler()];
	}
    
    mheGLView* glview() const
    {
        return glview_;
    }
private:
	void update()
	{
		[glview_ setViewEventsHandler:events_handler()];
	}

	mheGLView* glview_;
};

}}

#endif
