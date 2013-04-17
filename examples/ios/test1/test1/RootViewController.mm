//
//  RootViewController.m
//  test1
//
//  Created by skraevsky on 04.12.12.
//  Copyright (c) 2012 mht. All rights reserved.
//

#import "RootViewController.h"

@interface RootViewController ()
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

-(void) loadView
{
    CGRect rect = [UIScreen mainScreen].bounds;
    UIView* view = [[[UIView alloc] initWithFrame:rect] autorelease];
    self.view = view;  
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
