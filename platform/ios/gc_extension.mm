#include "sb_ios_extension.h"
#include "sb_application.h"
#include "json/sb_json.h"
#include <ghl_system.h>
#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@interface gc_manager : NSObject {
@public
    Sandbox::Application* m_application;
}
@end

@implementation gc_manager

-(id)init:(Sandbox::Application*) app {
    if (self = [super init]) {
        m_application = app;
        NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
        [nc addObserver: self
               selector:@selector(gameCenterAuthenticationChanged:)
                   name:GKPlayerAuthenticationDidChangeNotificationName
                 object:nil];
    }
    return self;
}
-(void)dealloc {
    [super dealloc];
}

- (void) authenticatedPlayer:(GKPlayer*) player
{
    NSLog(@"[GC] authenticatedPlayer: %@",player);
    if ([GKLocalPlayer localPlayer].authenticated) {
        m_application->OnExtensionResponse("GCLogin","success");
    }
}

- (void) disableGameCenter:(NSError*) error
{
    NSLog(@"[GC] disableGameCenter: %@",error);
    m_application->OnExtensionResponse("GCLogin","failed");
}

- (void) showAuthenticationDialogWhenReasonable:(UIViewController*) viewController
{
    NSLog(@"[GC] showAuthenticationDialogWhenReasonable");
    UIViewController* main_controller = 0;
    GHL::System* system = m_application->GetSystem();
    sb_assert(system);
    if (system->GetDeviceData(GHL::DEVICE_DATA_VIEW_CONTROLLER, &main_controller) && main_controller) {
        [main_controller presentViewController:viewController animated:YES completion:nil];
    }
}

- (void) authenticateLocalPlayer
{
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    localPlayer.authenticateHandler = ^(UIViewController *viewController, NSError *error){
        if (viewController != nil)
        {
            //showAuthenticationDialogWhenReasonable: is an example method name. Create your own method that displays an authentication view when appropriate for your app.
            [self showAuthenticationDialogWhenReasonable: viewController];
        }
        else if (localPlayer.isAuthenticated)
        {
            //authenticatedPlayer: is an example method name. Create your own method that is called after the local player is authenticated.
            [self authenticatedPlayer: localPlayer];
        }
        else
        {
            [self disableGameCenter: error];
        }
    };
}

-(void)gameCenterAuthenticationChanged:(id) object {
    NSLog(@"[GC] gameCenterAuthenticationChanged %@",object);
}

-(BOOL) isAuthenticated {
    return [GKLocalPlayer localPlayer].authenticated;
}
-(sb::string) dumpPlayer {
    GKLocalPlayer* player = [GKLocalPlayer localPlayer];
    return Sandbox::JsonBuilder()
        .BeginObject()
            .Key("id").PutString([[player playerID] UTF8String])
            .Key("name").PutString([[player displayName] UTF8String])
        .EndObject()
    .End();
}
@end


class gc_platform_extension : public Sandbox::iOSPlatformExtension {
private:
    gc_manager* m_mgr;
public:
    gc_platform_extension() : m_mgr(0){
        
    }
    ~gc_platform_extension() {
        [m_mgr release];
    }
    
    virtual void OnAppStarted(Sandbox::Application* app){
        m_mgr = [[gc_manager alloc] init:app];
    }
    
    virtual bool Process(Sandbox::Application* app,
                         const char* method,
                         const char* args,
                         sb::string& res){
        if (!m_mgr)
            return false;
        if (::strcmp(method,"GCLogin")==0) {
            if ([m_mgr isAuthenticated]) {
                res = "success";
                return true;
            }
            [m_mgr authenticateLocalPlayer];
            res = "pending";
            return true;
        }
        if (::strcmp(method,"GCGetPlayer")==0) {
            if (![m_mgr isAuthenticated]) {
                res = "";
                return true;
            }
            res = [m_mgr dumpPlayer];
            return true;
        }
        return false;
    }
};

gc_platform_extension __gc_extension;