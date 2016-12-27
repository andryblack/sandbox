#include "sb_ios_extension.h"
#include "sb_application.h"
#include "json/sb_json.h"
#include <ghl_system.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "utils/sb_base64.h"
#include "json/sb_json.h"

#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)

@interface pn_manager : NSObject<UIApplicationDelegate> {
@public
    Sandbox::Application* m_application;
}
@end

@implementation pn_manager

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    NSLog(@"didRegisterForRemoteNotificationsWithDeviceToken");
    if (m_application) {
        sb::string token;
        if (deviceToken) {
            size_t len = deviceToken.length;
            token.resize(len*2, '0');
            const GHL::Byte* data = static_cast<const GHL::Byte*>(deviceToken.bytes);
            for (size_t i=0;i<len;++i) {
                char buf[8];
                snprintf(buf, 8, "%02X", (int)data[i]);
                token[i*2+0]=buf[0];
                token[i*2+1]=buf[1];
            }
        }
        //= Sandbox::Base64EncodeData(static_cast<const GHL::Byte*>(deviceToken.bytes),deviceToken.length);
        sb::string res = Sandbox::JsonBuilder()
            .BeginObject()
                .Key("status").PutString("success")
                .Key("token").PutString(token.c_str())
            .EndObject()
        .End();
        m_application->OnExtensionResponse("PNGetToken", res.c_str());
    }
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
    NSLog(@"didFailToRegisterForRemoteNotificationsWithError: %@",error);
    sb::string res = Sandbox::JsonBuilder()
        .BeginObject()
            .Key("status").PutString("failed")
            .Key("error").PutString([error description].UTF8String)
        .EndObject()
    .End();
    m_application->OnExtensionResponse("PNGetToken", res.c_str());
}

-(void)registerForNotifications {
    if (SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(@"8.0")) {
        [[UIApplication sharedApplication] registerForRemoteNotifications];
        [[UIApplication sharedApplication] registerUserNotificationSettings:
         [UIUserNotificationSettings settingsForTypes:(
                                                       UIUserNotificationTypeBadge |
                                                       UIUserNotificationTypeSound |
                                                       UIUserNotificationTypeAlert) categories: nil]];
    } else {
        [[UIApplication sharedApplication] registerForRemoteNotificationTypes:(
                                                                               UIRemoteNotificationTypeBadge |
                                                                               UIRemoteNotificationTypeSound |
                                                                               UIRemoteNotificationTypeAlert)];
        
    }
}

-(id)init:(Sandbox::Application*) app {
    if (self = [super init]) {
        m_application = app;
        //NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
        //[self registerForNotifications];
    }
    return self;
}
-(void)dealloc {
    [super dealloc];
}

@end


class pn_platform_extension : public Sandbox::iOSPlatformExtension {
private:
    pn_manager* m_mgr;
public:
    pn_platform_extension() : m_mgr(0){
        
    }
    ~pn_platform_extension() {
        [m_mgr release];
    }
    
    virtual void OnAppStarted(Sandbox::Application* app){
        m_mgr = [[pn_manager alloc] init:app];
    }
    
    virtual NSObject* GetObject() { return m_mgr; }
    
    virtual bool Process(Sandbox::Application* app,
                         const char* method,
                         const char* args,
                         sb::string& res){
        if (!m_mgr)
            return false;
        if (::strcmp("PNGetToken",method)==0) {
            [m_mgr registerForNotifications];
            res = "{\"status\":\"pending\"}";
            return true;
        }
        
        return false;
    }
};


extern "C" void *init_pn_extension() {
    static pn_platform_extension __pn_extension;
    return &__pn_extension;
}
