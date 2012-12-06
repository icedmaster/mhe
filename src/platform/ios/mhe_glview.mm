#include "platform/ios/mhe_glview.h"

#include "utils/unused.hpp"

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

-(void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    UNUSED(view); UNUSED(rect);
    if (engine_ != nullptr)
        engine_->render();
}

@end
