#ifndef SB_IOS_EXTENSION_H_INCLUDED
#define SB_IOS_EXTENSION_H_INCLUDED

#include "sb_extension.h"
#import <Foundation/Foundation.h>

namespace Sandbox {
    
    class iOSPlatformExtension : public PlatformExtension {
    private:
    public:
        iOSPlatformExtension();
        ~iOSPlatformExtension();
        
        virtual NSObject* GetObject() { return 0; }
        
        static iOSPlatformExtension* GetRoot() { return static_cast<iOSPlatformExtension*>(PlatformExtension::GetRoot()); }
        iOSPlatformExtension* GetNext() { return static_cast<iOSPlatformExtension*>(PlatformExtension::GetNext()); }
    };

}

#endif /*SB_IOS_EXTENSION_H_INCLUDED*/
