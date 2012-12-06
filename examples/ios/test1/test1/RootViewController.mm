//
//  RootViewController.m
//  test1
//
//  Created by skraevsky on 04.12.12.
//  Copyright (c) 2012 mht. All rights reserved.
//

#import "RootViewController.h"
#import <QuartzCore/QuartzCore.h>
#include <mhe.hpp>
#include <platform/ios/ios_window_system_impl.hpp>
#include "TestScene.h"

@interface RootViewController ()
{
    mhe::game::Engine* engine_;
}

@property (nonatomic, retain) NSTimer* timer;
@property (nonatomic, retain) CADisplayLink* displayLink;
@end

@implementation RootViewController

@synthesize timer;
@synthesize displayLink;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

-(id) init
{
    self = [super init];
    engine_ = new mhe::game::Engine;
    return self;
}

-(void) dealloc
{
    delete engine_;
    [timer release];
    [super dealloc];
}

-(void) loadView
{
    CGRect rect = [UIScreen mainScreen].bounds;
    UIView* view = [[[UIView alloc] initWithFrame:rect] autorelease];
    self.view = view;  
}

-(NSString*) textureBasePath
{
    NSString* path=[[NSBundle mainBundle] resourcePath];
    return path;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    CGRect rect = self.view.frame;
    engine_->init(rect.size.width, rect.size.height, 32, false);
    const mhe::ios::iOSSurface* surface = static_cast<const mhe::ios::iOSSurface*>(engine_->context().window_system().surface());
    [self.view addSubview:surface->view()];
    [surface->delegate() setEngine:engine_];
    std::string last = "/";
    std::string path = [[self textureBasePath] fileSystemRepresentation];
    path += last;
    engine_->context().texture_manager().set_base_path(path);
    
    boost::shared_ptr<TestScene> test_scene(new TestScene(engine_));
    test_scene->init("");
    engine_->set_game_scene(test_scene);
    engine_->game_scene()->scene()->add_camera(new mhe::Camera2D(rect.size.width, rect.size.height), true);
    engine_->context().driver()->set_clear_color(mhe::color_green);
	// Do any additional setup after loading the view.
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(onTimer)];
    self.displayLink.frameInterval = 1.0f / 60.0f;
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void) onTimer
{
    engine_->update();
    const mhe::ios::iOSSurface* surface = static_cast<const mhe::ios::iOSSurface*>(engine_->context().window_system().surface());
    [surface->view() display];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
