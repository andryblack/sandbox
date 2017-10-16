#ifndef SB_IOS_EXTENSION_H_INCLUDED
#define SB_IOS_EXTENSION_H_INCLUDED

#include "sb_extension.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIApplication.h>

namespace Sandbox {
    
    class iOSPlatformExtension : public PlatformExtension {
    private:
    public:
        iOSPlatformExtension();
        ~iOSPlatformExtension();
        
        virtual NSObject<UIApplicationDelegate>* GetObject() { return 0; }
        
        static iOSPlatformExtension* GetRoot() { return static_cast<iOSPlatformExtension*>(PlatformExtension::GetRoot()); }
        iOSPlatformExtension* GetNext() { return static_cast<iOSPlatformExtension*>(PlatformExtension::GetNext()); }
    };

}

#ifdef __OBJC__
#import "../../GHL/src/winlib/winlib_cocoatouch.h"
@interface SandboxAppDelegate : WinLibAppDelegate

@end
#endif

#endif /*SB_IOS_EXTENSION_H_INCLUDED*/
