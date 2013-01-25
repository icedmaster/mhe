#include "platform/ios/mhe_glview.h"

#include "utils/unused.hpp"
#include "view.hpp"
#include "mhe_event.hpp"

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

-(void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UNUSED(event);
    UITouch* touch = [touches anyObject];
    CGPoint point = [touch locationInView:self];
    _eventsHandler->on_mouse_click(mhe::Event::any_event, mhe::v2d(point.x, point.y));
}

@end
