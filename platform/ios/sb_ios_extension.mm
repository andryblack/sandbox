#include "sb_ios_extension.h"
#import <UIKit/UIKit.h>
#import <objc/runtime.h>

static IMP super_imp(id rcv, SEL a_sel, IMP origin_imp)
{
    IMP ret = NULL;
    Class klass = [rcv class];
    
    while ((klass = [klass superclass])) {
        ret = [klass instanceMethodForSelector: a_sel];
        if (ret && ret != origin_imp)
            return ret;
    }
    return NULL;
}

static void imp_forwardInvocation(id rcv, SEL method,NSInvocation *anInvocation)
{
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
    if (!handled) {
        IMP simp = super_imp(rcv, method, (IMP)imp_forwardInvocation);
        (*simp)(rcv, method, anInvocation);
    }
}

static BOOL imp_respondsToSelector (id rcv, SEL method, SEL arg0)
{
    BOOL ret;
    IMP simp = super_imp(rcv, method, (IMP)imp_respondsToSelector);
    
    ret = ((BOOL (*)(id, SEL, SEL))simp)(rcv, method, arg0);
    if (!ret) {
        Sandbox::iOSPlatformExtension* ext = Sandbox::iOSPlatformExtension::GetRoot();
        while (ext) {
            NSObject* obj = ext->GetObject();
            if (obj && [obj respondsToSelector:arg0]) {
                ret = TRUE;
                break;
            }
            ext = ext->GetNext();
        }
    }
    return ret;
}

static void extend_app_delegate() {
    Class c = objc_getClass("WinLibAppDelegate");
    if (c) {
        if (class_addMethod(c,@selector(forwardInvocation:),(IMP)imp_forwardInvocation,"v@:@")) {
            NSLog(@"add forward invocation");
        } else {
            NSLog(@"add forward invocation failed");
        }
        if (class_addMethod(c,@selector(respondsToSelector:),(IMP)imp_respondsToSelector,"c@::")) {
            NSLog(@"add respond to selector");
        } else {
            NSLog(@"add respond to selector failed");
        }
    }
}

namespace Sandbox {
    iOSPlatformExtension::iOSPlatformExtension() {
        static dispatch_once_t once;
        dispatch_once(&once, ^ {
            extend_app_delegate();
        });
    }
    
    iOSPlatformExtension::~iOSPlatformExtension() {
        
    }
}


