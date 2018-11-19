#include "sb_ios_extension.h"
#include "sb_application.h"
#include "json/sb_json.h"
#include <ghl_system.h>
#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@interface gc_manager : NSObject <GKGameCenterControllerDelegate> {
@public
    Sandbox::Application* m_application;
    bool m_nees_subscibe;
}
@end

@implementation gc_manager

-(id)init:(Sandbox::Application*) app {
    if (self = [super init]) {
        m_application = app;
        m_nees_subscibe = true;
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

- (void) authenticateLocalPlayer:(BOOL)forceShowUI
{
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    if (forceShowUI && localPlayer.authenticateHandler) {
        if (![UIApplication.sharedApplication openURL:[NSURL URLWithString:@"gamecenter:/me/account"]]) {
            UIViewController* main_controller = 0;
            GHL::System* system = m_application->GetSystem();
            if (system->GetDeviceData(GHL::DEVICE_DATA_VIEW_CONTROLLER, &main_controller) && main_controller) {
                GKGameCenterViewController* gcViewController = [[GKGameCenterViewController alloc] init];
                
                gcViewController.gameCenterDelegate = self;
                
                gcViewController.viewState = GKGameCenterViewControllerStateDefault;
                
                [main_controller presentViewController:gcViewController animated:YES completion:nil];
                
            }
        }
        return;
    }
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
        
        if (m_nees_subscibe) {
            m_nees_subscibe = false;
            NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
            [nc addObserver: self
                   selector:@selector(gameCenterAuthenticationChanged:)
                       name:GKPlayerAuthenticationDidChangeNotificationName
                     object:nil];
        }
    };
}

-(void)gameCenterAuthenticationChanged:(id) object {
    NSLog(@"[GC] gameCenterAuthenticationChanged %@",object);
    if (m_application) m_application->OnExtensionResponse("GCLogin","changed");
}

-(BOOL) isAuthenticated {
    return [GKLocalPlayer localPlayer].authenticated;
}
-(sb::string) dumpPlayer {
    GKLocalPlayer* player = [GKLocalPlayer localPlayer];
    Sandbox::JsonBuilder json;
    json.BeginObject()
            .Key("id").PutString([[player playerID] UTF8String])
            .Key("name").PutString([[player displayName] UTF8String])
            .Key("nickname").PutString([[player alias] UTF8String])
    .EndObject();
    return json.End();
}

-(void) submitHighScores: (NSDictionary*) scores {
    NSMutableArray* scoresData = [NSMutableArray arrayWithCapacity:scores.count];
    for (NSString* key in scores.allKeys) {
        GKScore* score = [[[GKScore alloc] initWithLeaderboardIdentifier:key] autorelease];
        if (score) {
            score.value = [(NSNumber*)[scores objectForKey:key] longLongValue];
            [scoresData addObject:score];
            NSLog(@"[GC] submitHighScore to %@ : %lld",score.leaderboardIdentifier,score.value);
        }
    }   
    
    [GKScore reportScores:scoresData withCompletionHandler:^(NSError *error){
        if (error != nil) {
            NSLog(@"[GC] submitHighScore %@", [error description]);
        } else {
            NSLog(@"[GC] success");
        }
    }];
}

-(void) submitAchievements:(NSDictionary*) scores {
    NSMutableArray* achievementsData = [NSMutableArray arrayWithCapacity:scores.count];
    for (NSString* key in scores.allKeys) {
        GKAchievement* a = [[[GKAchievement alloc] initWithIdentifier:key] autorelease];
        a.percentComplete = [(NSNumber*)[scores objectForKey:key] intValue] * 1.0;
        a.showsCompletionBanner = YES;
        [achievementsData addObject:a];
        NSLog(@"[GC] submitAchievement to %@ : %f",a.identifier,a.percentComplete);
    }
    
    [GKAchievement reportAchievements:achievementsData withCompletionHandler:^(NSError* error) {
        if (error != nil) {
            NSLog(@"[GC] submitAchievements %@", [error description]);
        } else {
            NSLog(@"[GC] success");
        }
    }];
}


// need to dismiss score screen
-(void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
    [gameCenterViewController dismissViewControllerAnimated:YES completion:nil];
    [gameCenterViewController release];
}

-(sb::string) showScoresUI {
    UIViewController* main_controller = 0;
    GHL::System* system = m_application->GetSystem();
    if (system->GetDeviceData(GHL::DEVICE_DATA_VIEW_CONTROLLER, &main_controller) && main_controller) {
        GKGameCenterViewController* gcViewController = [[GKGameCenterViewController alloc] init];
        
        gcViewController.gameCenterDelegate = self;
        
        gcViewController.viewState = GKGameCenterViewControllerStateLeaderboards;
        gcViewController.leaderboardTimeScope = GKLeaderboardTimeScopeAllTime;
        
        [main_controller presentViewController:gcViewController animated:YES completion:nil];

        return "success";
    }
    return "error";
}

-(sb::string) showAchievementsUI {
    UIViewController* main_controller = 0;
    GHL::System* system = m_application->GetSystem();
    if (system->GetDeviceData(GHL::DEVICE_DATA_VIEW_CONTROLLER, &main_controller) && main_controller) {
        GKGameCenterViewController* gcViewController = [[GKGameCenterViewController alloc] init];
        
        gcViewController.gameCenterDelegate = self;
        
        gcViewController.viewState = GKGameCenterViewControllerStateAchievements;
        gcViewController.leaderboardTimeScope = GKLeaderboardTimeScopeAllTime;
        
        [main_controller presentViewController:gcViewController animated:YES completion:nil];
        
        return "success";
    }
    return "error";
}


-(sb::string) dumpFriends:(NSArray<NSString *>*) friends {
    Sandbox::JsonBuilder res;
    res.BeginArray();
    for (NSString* _id in friends) {
        res.BeginObject();
        res.Key("id").PutString(_id.UTF8String);
        res.EndObject();
    }
    res.EndArray();
    return res.End();
};

-(sb::string) requestFriends {
    GKLocalPlayer* player = [GKLocalPlayer localPlayer];
    NSArray<NSString *>* friends = [player friends];
    if (friends) {
        return [self dumpFriends:friends];
    }
    if ([player respondsToSelector:@selector(loadFriendsWithCompletionHandler:)]) {
        [player loadFriendsWithCompletionHandler:^(NSArray<NSString *> * _Nullable friendIDs, NSError * _Nullable error) {
            if (friendIDs && !error) {
            
                if (m_application) m_application->OnExtensionResponse("GCGetFriends",[self dumpFriends:friendIDs].c_str());
            } else {
                NSLog(@"loadFriends failed: %@",error);
                if (m_application) m_application->OnExtensionResponse("GCGetFriends","failed");
            }
        }];
    } else {
        return "failed";
    }
    return "pending";
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
        if (::strcmp("GCSendScores",method)==0) {
            if (![m_mgr isAuthenticated]) {
                res = "failed";
                return true;
            }
            NSData* argsData = [NSData dataWithBytes:args length:strlen(args)];
            NSDictionary* data = [NSJSONSerialization JSONObjectWithData:argsData options:0 error:nil];
            if (data) {
                [m_mgr submitHighScores:data];
                res = "success";
            } else {
                res = "failed";
            }
            return true;
        }
        if (::strcmp("GCSendAchievements",method)==0) {
            if (![m_mgr isAuthenticated]) {
                res = "failed";
                return true;
            }
            NSData* argsData = [NSData dataWithBytes:args length:strlen(args)];
            NSDictionary* data = [NSJSONSerialization JSONObjectWithData:argsData options:0 error:nil];
            if (data) {
                [m_mgr submitAchievements:data];
                res = "success";
            } else {
                res = "failed";
            }
            return true;
        }

        if (::strcmp(method,"GCLogin")==0) {
            if ([m_mgr isAuthenticated]) {
                res = "success";
                return true;
            }
            [m_mgr authenticateLocalPlayer:(args && ::strcmp(args,"force_ui")==0)];
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
        if (::strcmp(method,"GCGetFriends")==0) {
            if (![m_mgr isAuthenticated]) {
                res = "failed";
                return true;
            }
            res = [m_mgr requestFriends];
            return true;
        }
        if (::strcmp("GCShowUI",method)==0) {
            if (![m_mgr isAuthenticated]) {
                res = "";
                return true;
            }
            if (::strcmp("leaderboards",args)==0) {
                res = [m_mgr showScoresUI];
            } else if (::strcmp("achievements",args)==0) {
                res = [m_mgr showAchievementsUI];
            }
            
            return true;
        }
        return false;
    }
};


extern "C" void *init_gc_extension() {
    static gc_platform_extension __gc_extension;
    return &__gc_extension;
}
