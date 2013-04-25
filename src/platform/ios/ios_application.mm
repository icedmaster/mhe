#include "platform/ios/ios_application.hpp"
#include "platform/ios/ios_view.hpp"
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

@interface iOSApplicationImpl : NSObject
{
    CADisplayLink* _displayLink;
    mhe::game::Engine* _engine;
}
-(void) configure:(mhe::game::Engine*)engine;
@end

@implementation iOSApplicationImpl

-(void) dealloc
{
    [_displayLink release];
    [super dealloc];
}

-(void) configure:(mhe::game::Engine*)engine
{
    _engine = engine;
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(onTimer)];
    _displayLink.frameInterval = 1.0f / 60.0f;
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void) onTimer
{
    _engine->update();
    const mhe::ios::iOSView* view = static_cast<const mhe::ios::iOSView*>(_engine->context().window_system().view());
    [view->glview() display];
}

@end

namespace mhe {
namespace app {
namespace ios {

namespace {

inline std::string bundle_path()
{
	std::string path = [[[NSBundle mainBundle] resourcePath] fileSystemRepresentation];
	path += '/';
	return path;
}

}

iOSApplication::iOSApplication()
{
    utils::ios::create_standart_log();
}

bool iOSApplication::mhe_app_init(const ApplicationConfig& config)
{
	if (!Application::mhe_app_init(config))
		return false;

    impl_ = [[iOSApplicationImpl alloc] init];
    [impl_ configure:&engine()];

	return true;
}

void iOSApplication::mhe_app_deinit()
{
    [impl_ release];
}

std::string iOSApplication::application_base_path() const
{
	return bundle_path();
}

}}}
