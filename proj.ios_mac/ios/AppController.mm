/****************************************************************************
 Copyright (c) 2010-2013 cocos2d-x.org
 Copyright (c) 2013-2017 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#import "AppController.h"
#import "cocos2d.h"
#import "AppDelegate.h"
#import "RootViewController.h"

// Crashlytics
#import <Fabric/Fabric.h>
#import <Crashlytics/Crashlytics.h>

// Facebook
#import <FBSDKCoreKit/FBSDKCoreKit.h>

// Twitter
#import <TwitterKit/TWTRKit.h>

// MoPub Ads
#import "MoPub.h"

// Firebase (for AdMob conversion tracking)
#import "FIRApp.h"

@implementation AppController

@synthesize window;

#pragma mark -
#pragma mark Application lifecycle

// cocos2d application instance
static AppDelegate s_sharedApplication;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    // Firebase (for AdMob conversion tracking)
    // This MUST be initialized before Fabric or you will get an error: "The default Firebase app has not yet been configured."
    [FIRApp configure];
    
    // Crashlytics
    [Fabric with:@[[Crashlytics class]]];
    
    // Facebook
    BOOL ret = [[FBSDKApplicationDelegate sharedInstance] application:application
                                        didFinishLaunchingWithOptions:launchOptions];
    
    // Twitter
    [[Twitter sharedInstance] startWithConsumerKey:@"ICTLqKk0SILDbt8UUDhfv2PNp" consumerSecret:@"zBqGiYgphiPsu5HYtbKomI4XCxcxPvGsPe9JykYTYOqeKBlUQF"];
    
    // MoPub Ads
    // I think you only need one of the ad unit IDs here
    // other add units will be served from the IDs in AdMob Mediation
    MPMoPubConfiguration *sdkConfig =
    [[MPMoPubConfiguration alloc] initWithAdUnitIdForAppInitialization:@"f16ebaf82197436289036410e01f6ec2"];
    
    [[MoPub sharedInstance] initializeSdkWithConfiguration:sdkConfig completion:nil];
    
    cocos2d::Application *app = cocos2d::Application::getInstance();
    
    // Initialize the GLView attributes
    app->initGLContextAttrs();
    cocos2d::GLViewImpl::convertAttrs();
    
    // Override point for customization after application launch.

    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];

    // Use RootViewController to manage CCEAGLView
    _viewController = [[RootViewController alloc]init];
    
    // hwmod; fix deprecation
    // _viewController.wantsFullScreenLayout = YES;
    _viewController.edgesForExtendedLayout = UIRectEdgeAll;

    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: _viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:_viewController];
    }

    [window makeKeyAndVisible];
    
    // IMPORTANT: Setting the GLView should be done after creating the RootViewController
    cocos2d::GLView *glview = cocos2d::GLViewImpl::createWithEAGLView((__bridge void *)_viewController.view);
    cocos2d::Director::getInstance()->setOpenGLView(glview);
    
    //run the cocos2d-x game scene
    //app->run() must be invoked after fb
    app->run();

    return ret;
}

- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<NSString *, id> *)options {
    // try to handle the URL with Twitter
    if ([[Twitter sharedInstance] application:app openURL:url options:options]) {
        return YES;
    }
    
    // if Twitter can't handle the URL, try with Facebook
    NSString* sourceApplication = [options objectForKey:UIApplicationOpenURLOptionsSourceApplicationKey];
    return [[FBSDKApplicationDelegate sharedInstance] application:app
                                                          openURL:url
                                                sourceApplication:sourceApplication
                                                       annotation:nil];
}

// When sharing with Facebook, a browser modal view will be presented, and Facebook will return to the app
// using one of its fb URL schemes

// handled above instead to prevent a conflict with Twitter
//- (BOOL)application:(UIApplication *)application
//            openURL:(NSURL *)url
//  sourceApplication:(NSString *)sourceApplication
//         annotation:(id)annotation {
//
//    return [[FBSDKApplicationDelegate sharedInstance] application:application
//                                                          openURL:url
//                                                sourceApplication:sourceApplication
//                                                       annotation:annotation];
//}

- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    // We don't need to call this method any more. It will interrupt user defined game pause&resume logic
    /* cocos2d::Director::getInstance()->pause(); */
    
    // see note below
    cocos2d::Application::getInstance()->applicationDidEnterBackground();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    // We don't need to call this method any more. It will interrupt user defined game pause&resume logic
    /* cocos2d::Director::getInstance()->resume(); */
    
    [FBSDKAppEvents activateApp];
    
    // see note below
    cocos2d::Application::getInstance()->applicationWillEnterForeground();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    
    // moved to the more sensitive -applicationWillResignActive:, which will be called for more minor interruptions,
    // such as swiping up from the bottom of the screen
    // cocos2d::Application::getInstance()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    
    // moved to the more sensitive -applicationDidBecomeActive:, which will be called for more minor interruptions,
    // such as swiping up from the bottom of the screen
    // cocos2d::Application::getInstance()->applicationWillEnterForeground();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}


#if __has_feature(objc_arc)
#else
- (void)dealloc {
    [window release];
    [_viewController release];
    [super dealloc];
}
#endif


@end
