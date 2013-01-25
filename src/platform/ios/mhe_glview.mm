#include "platform/ios/mhe_glview.h"

#include "utils/unused.hpp"
#include "view.hpp"

@interface mheGLView()
{
	mhe::ViewEventsHandler* _eventsHandler;
}
@end

@implementation mheGLView

-(void) setViewEventsHandler:(mhe::ViewEventsHandler*)eventsHandler
{
	_eventsHandler = eventsHandler;
}

-(void) drawRect:(CGRect)rect
{
    UNUSED(rect);
    [super drawRect:rect];
    assert(_eventsHandler != nullptr);
	_eventsHandler->on_draw();
}

@end
