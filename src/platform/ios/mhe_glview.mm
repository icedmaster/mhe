#include "platform/ios/mhe_glview.h"

@interface mheGLView()
{
	mhe::game::Engine* engine_;
}
@end

@implementation mheGLView

-(void) setEngine:(mhe::game::Engine*)engine
{
	engine_ = engine;
}

-(void) drawRect:(CGRect)rect
{
    (void)rect;
    if (engine_ != nullptr)
        engine_->process();
}

@end
