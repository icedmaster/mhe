//
//  RootViewController.m
//  test1
//
//  Created by skraevsky on 04.12.12.
//  Copyright (c) 2012 mht. All rights reserved.
//

#import "RootViewController.h"
#include <mhe.hpp>
#include <platform/ios/ios_window_system_impl.hpp>

@interface RootViewController ()
{
    mhe::game::Engine* engine_;
}
@end

@implementation RootViewController

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
    [super dealloc];
}

-(void) loadView
{
    CGRect rect = [UIScreen mainScreen].bounds;
    UIView* view = [[[UIView alloc] initWithFrame:rect] autorelease];
    self.view = view;
    engine_->init(rect.size.width, rect.size.height, 32, false);
    const mhe::ios::iOSSurface* surface = static_cast<const mhe::ios::iOSSurface*>(engine_->context().window_system().surface());
    [self.view addSubview:surface->get()];
    [surface->get() setEngine:engine_];
    engine_->context().driver()->set_clear_color(mhe::color_red);
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
