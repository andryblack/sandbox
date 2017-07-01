#include "sb_ios_extension.h"
#import <UIKit/UIKit.h>
#import "../../GHL/src/winlib/winlib_cocoatouch.h"

@interface SandboxAppDelegate : WinLibAppDelegate

@end

@implementation SandboxAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary *)launchOptions {
    [super application:application didFinishLaunchingWithOptions:launchOptions];
    Sandbox::iOSPlatformExtension* ext = Sandbox::iOSPlatformExtension::GetRoot();
    while (ext) {
        NSObject* obj = ext->GetObject();
        if (obj && [obj respondsToSelector:@selector(application:didFinishLaunchingWithOptions:)]) {
            [obj performSelector:@selector(application:didFinishLaunchingWithOptions:) withObject:application withObject:launchOptions];
        }
        ext = ext->GetNext();
    }
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    [super applicationWillResignActive:application];
    Sandbox::iOSPlatformExtension* ext = Sandbox::iOSPlatformExtension::GetRoot();
    while (ext) {
        NSObject* obj = ext->GetObject();
        if (obj && [obj respondsToSelector:@selector(applicationWillResignActive:)]) {
            [obj performSelector:@selector(applicationWillResignActive:) withObject:application];
        }
        ext = ext->GetNext();
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [super applicationDidBecomeActive:application];
    Sandbox::iOSPlatformExtension* ext = Sandbox::iOSPlatformExtension::GetRoot();
    while (ext) {
        NSObject* obj = ext->GetObject();
        if (obj && [obj respondsToSelector:@selector(applicationDidBecomeActive:)]) {
            [obj performSelector:@selector(applicationDidBecomeActive:) withObject:application];
        }
        ext = ext->GetNext();
    }
}

#if __IPHONE_OS_VERSION_MAX_ALLOWED > __IPHONE_9_3
- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
            options:(NSDictionary<UIApplicationOpenURLOptionsKey,id> *)options {
    BOOL handled = [super application:application openURL:url options:options];
    Sandbox::iOSPlatformExtension* ext = Sandbox::iOSPlatformExtension::GetRoot();
    while (ext) {
        NSObject<UIApplicationDelegate>* obj = ext->GetObject();
        if (obj && [obj respondsToSelector:@selector(application:openURL:options:)]) {
            if ([obj application:application openURL:url options:options]) {
                handled = YES;
            }
        }
        ext = ext->GetNext();
    }
    return handled;
}
#endif
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(nullable NSString *)sourceApplication annotation:(id)annotation {
    BOOL handled = [super application:application openURL:url sourceApplication:sourceApplication annotation:annotation];
    Sandbox::iOSPlatformExtension* ext = Sandbox::iOSPlatformExtension::GetRoot();
    while (ext) {
        NSObject<UIApplicationDelegate>* obj = ext->GetObject();
        if (obj && [obj respondsToSelector:@selector(application:openURL:sourceApplication:annotation:)]) {
            if ([obj application:application openURL:url sourceApplication:sourceApplication annotation:annotation]) {
                handled = YES;
            }
        }
        ext = ext->GetNext();
    }
    return handled;
}



-(void)forwardInvocation:(NSInvocation *)anInvocation {
    Sandbox::iOSPlatformExtension* ext = Sandbox::iOSPlatformExtension::GetRoot();
    bool handled = false;
    while (ext) {
        NSObject* obj = ext->GetObject();
        if (obj && [obj respondsToSelector:[anInvocation selector]]) {
            [anInvocation invokeWithTarget:obj];
            handled = true;
        }
        ext = ext->GetNext();
    }
    if ([super respondsToSelector:[anInvocation selector]]) {
        [super forwardInvocation: anInvocation];
    }
}

-(BOOL)respondsToSelector:(SEL)aSelector {
    BOOL ret = [super respondsToSelector:aSelector];
    if (!ret) {
        Sandbox::iOSPlatformExtension* ext = Sandbox::iOSPlatformExtension::GetRoot();
        while (ext) {
            NSObject* obj = ext->GetObject();
            if (obj && [obj respondsToSelector:aSelector]) {
                ret = TRUE;
                break;
            }
            ext = ext->GetNext();
        }
    }
    return ret;
}

@end

namespace Sandbox {
    iOSPlatformExtension::iOSPlatformExtension() {
       
    }
    
    iOSPlatformExtension::~iOSPlatformExtension() {
        
    }
}


