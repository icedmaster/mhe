//
//  AppDelegate.m
//  test1
//
//  Created by skraevsky on 04.12.12.
//  Copyright (c) 2012 mht. All rights reserved.
//

#import "AppDelegate.h"
#import "RootViewController.h"
#include <platform/ios/ios_application.hpp>
#include <platform/ios/ios_view.hpp>
#include "TestScene.h"

@interface AppDelegate()
{
    mhe::app::ios::iOSApplication* application_;
}

@property (nonatomic, retain) RootViewController* rootViewController;

@end

@implementation AppDelegate

@synthesize rootViewController;

- (void)dealloc
{
    delete application_;
    [_window release];
    [rootViewController release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    application_ = new mhe::app::ios::iOSApplication();
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // TODO: application config can be loaded here
    mhe::app::ApplicationConfig config;
    config.width = self.window.bounds.size.width;
    config.height = self.window.bounds.size.height;
    config.bpp = 32;
    application_->init(config);
    // Override point for customization after application launch.
    self.rootViewController = [[[RootViewController alloc] init] autorelease];
    self.window.rootViewController = self.rootViewController;
    mhe::ios::iOSView* view = static_cast<mhe::ios::iOSView*>(application_->engine().context().window_system().view());
    [self.rootViewController.view addSubview:view->glview()];
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];

    [self userInitialization];

    // Do any additional setup after loading the view.
    return YES;
}

-(void) userInitialization
{
    // write your own code here
    application_->engine().event_manager().add_device(new mhe::MouseDevice("mouse"));

    application_->engine().context().texture_manager().set_path(application_->path());
    application_->engine().font_manager().set_path(application_->path());

    boost::shared_ptr<TestScene> test_scene(new TestScene(&(application_->engine())));
    test_scene->init("");
    // add 2d camera
    const mhe::vector2<int>& size = application_->engine().context().window_system().screen_size();
    test_scene->scene()->add_camera(new mhe::Camera2D(size.x(), size.y()), true);
    application_->engine().set_game_scene(test_scene);
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
