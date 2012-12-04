#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#include "game/engine.hpp"

@interface mheGLView : GLKView

-(void) setEngine:(mhe::game::Engine*)engine;

@end
