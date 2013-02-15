#ifndef __MHE_GLVIEW_H__
#define __MHE_GLVIEW_H__

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#include "view.hpp"
#include "events/events_provider.hpp"

class ViewEventsHandler;

@interface mheGLView : GLKView

-(void) setViewEventsHandler:(mhe::ViewEventsHandler*)handler;
-(mhe::EventsProvider*) eventsProvider;

@end

#endif
