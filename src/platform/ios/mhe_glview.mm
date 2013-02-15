#include "platform/ios/mhe_glview.h"

#include "utils/unused.hpp"
#include "view.hpp"
#include "mhe_event.hpp"

@interface mheGLView()
{
	mhe::ViewEventsHandler* _eventsHandler;
	mhe::EventsProvider _eventsProvider;
}
@end

@implementation mheGLView

-(void) setViewEventsHandler:(mhe::ViewEventsHandler*)eventsHandler
{
	_eventsHandler = eventsHandler;
}

-(mhe::EventsProvider*) eventsProvider
{
	return &_eventsProvider;
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
    _eventsProvider.add_mouse_event(mhe::MouseEvent::button_pressed,
		mhe::Event::any_event, mhe::v2d(point.x, point.y));
}

-(void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UNUSED(event);
    UITouch* touch = [touches anyObject];
    CGPoint point = [touch locationInView:self];
    _eventsProvider.add_mouse_event(mhe::MouseEvent::move,
                                    mhe::Event::any_event, mhe::v2d(point.x, point.y));
}

-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UNUSED(event);
    UITouch* touch = [touches anyObject];
    CGPoint point = [touch locationInView:self];
    _eventsProvider.add_mouse_event(mhe::MouseEvent::button_released,
                                  mhe::Event::any_event, mhe::v2d(point.x, point.y));
}

@end
