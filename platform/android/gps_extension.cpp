

#include <sb_extension.h>
#include <sb_log.h>
#include <sb_application.h>
#include <ghl_system.h>
#include <json/sb_json.h>
#include <sbstd/sb_map.h>

#include "sb_android_extension.h"

#include "gpg/builder.h"
#include "gpg/game_services.h"
#include "gpg/leaderboard.h"
#include "gpg/leaderboard_manager.h"
#include "gpg/platform_configuration.h"
#include "gpg/player_manager.h"
#include "gpg/score_page.h"
#include "gpg/types.h"
#include "gpg/debug.h"
#include "gpg/default_callbacks.h"
#include "gpg/android_initialization.h"
#include "gpg/android_support.h"
#include "gpg/score_summary.h"

#include <android/native_activity.h>

static sb::string is_auth_in_progress_;
static std::unique_ptr<gpg::GameServices> game_services_;
static sb::string me_player_id;

void BeginUserInitiatedSignIn() {
    if (!game_services_)
        return;
    if (!game_services_->IsAuthorized()) {
        Sandbox::LogInfo() << "StartAuthorizationUI";
        game_services_->StartAuthorizationUI();
    }
}


void SubmitHighScore(char const *leaderboard_id, uint64_t score) {
    if (!game_services_)
        return;
    if (game_services_->IsAuthorized()) {
        Sandbox::LogInfo() << "High score submitted " << score << " " << leaderboard_id;
        game_services_->Leaderboards().SubmitScore(leaderboard_id, score);
        game_services_->Leaderboards().Fetch(leaderboard_id,nullptr);
    } else {
        BeginUserInitiatedSignIn();
    }
}

sb::string ShowScoresUI() {
    if (!game_services_) {
        Sandbox::LogError() << "ShowScoresUI error";
        return "error";
    }
    if (game_services_->IsAuthorized()) {
        Sandbox::LogInfo() << "High score show";
        //game_services_->Leaderboards().FetchAllBlocking(gpg::DataSource::CACHE_OR_NETWORK);
        game_services_->Leaderboards().ShowAllUI(nullptr);
        return "success";
    } else {
        BeginUserInitiatedSignIn();
    }
    return "error";
}

void InitServices(gpg::PlatformConfiguration const &pc) {
    Sandbox::LogInfo() << "Initializing Services";
    if (!game_services_) {
        Sandbox::LogInfo() << "Uninitialized services, so creating";
        game_services_ = gpg::GameServices::Builder()
        .SetOnLog(gpg::DEFAULT_ON_LOG, gpg::LogLevel::VERBOSE)
        .SetOnAuthActionStarted([](gpg::AuthOperation op) {
            is_auth_in_progress_ = "progress";
        })
        .SetOnAuthActionFinished([](gpg::AuthOperation op,
                                                     gpg::AuthStatus status) {
            Sandbox::LogInfo() << "Sign in finished with a result of " << status;
           
            if (gpg::IsSuccess(status)) {
                game_services_->Players().FetchSelf([op,status](const gpg::PlayerManager::FetchSelfResponse& resp) {
                    if (gpg::IsSuccess(resp.status)) {
                        is_auth_in_progress_ = "succcess";
                        me_player_id = resp.data.Id();
                    } else {
                        is_auth_in_progress_ = "failed";
                    }
                    game_services_->Leaderboards().FetchAll(nullptr);
                });
            } else {
                is_auth_in_progress_ = "failed";
            }
            
            
//            Sandbox::LogInfo() <<"Fetching all nonblocking";
//            game_services_->Achievements().FetchAll(gpg::DataSource::CACHE_OR_NETWORK, [] (gpg::AchievementManager::FetchAllResponse response) {
//                    Sandbox::LogInfo() << "Achievement response status: " << response.status);
//                });
//            Sandbox::LogInfo() <<"--------------------------------------------------------------";
            
        })
        .Create(pc);
    }
    Sandbox::LogInfo() << "Created";
}

class GPSExtension : public Sandbox::AndroidPlatformExtension {
public:
    virtual void OnLoad(Sandbox::Application* app) {
        Sandbox::LogInfo() << "GPSExtension::OnLoad";
        ANativeActivity* activity = GetNativeActivity(app);
        if (activity) {
            // gpg-cpp: Set platform intiialization
            Sandbox::LogInfo() << "GPSExtension AndroidInitialization";
            gpg::AndroidInitialization::ANativeActivity_onCreate(activity,0,0);
            
            // Get the platform configuration.
            gpg::AndroidPlatformConfiguration platform_configuration;
            platform_configuration.SetActivity(activity->clazz);
            
            // Now, create the game service (see StateManager.cpp)
            // and pass in our callback
            InitServices(platform_configuration);
            
        } else {
            Sandbox::LogError() << "GPSExtension : not found activity";
        }
    }
    virtual bool Process(Sandbox::Application* app,
                         const char* method,
                         const char* args,
                         sb::string& res) {
        if (::strcmp("GPSSendScores",method)==0) {
            sb::map<sb::string,sb::string> data;
            if (Sandbox::json_parse_object(args,data)) {
                SubmitHighScore(data["id"].c_str(),::atoll(data["val"].c_str()));
                res = "success";
            }
            return true;
        }
        if (::strcmp("GPSLogin",method)==0) {
            BeginUserInitiatedSignIn();
            res = "pending";
            return true;
        }
        if (::strcmp("GPSGetPlayerId",method)==0) {
            if (!game_services_)
                return true;
            if (!game_services_->IsAuthorized())
                return true;
            res = me_player_id;
            return true;
        }
        if (::strcmp("GPSGetLoginStatus",method)==0) {
            res = "undefined";
            if (!game_services_)
                return true;
            if (!game_services_->IsAuthorized())
                return true;
            res = is_auth_in_progress_;
            return true;
        }
        if (::strcmp("GPSGetScores",method)==0) {
            res = "[]";
            return true;
        }
        if (::strcmp("GPSShowScores",method)==0) {
            res = ShowScoresUI();
            return true;
        }

        return false;
    }
    
    
    virtual void nativeOnActivityCreated(
                                         JNIEnv *env,
                                         jobject thiz,
                                         jobject activity,
                                         jobject saved_instance_state) {
        gpg::AndroidSupport::OnActivityCreated(env,activity,saved_instance_state);
    }
    
    virtual void nativeOnActivityDestroyed(
                                           JNIEnv *env, jobject thiz, jobject activity) {
        gpg::AndroidSupport::OnActivityDestroyed(env,activity);
    }
    
    virtual void nativeOnActivityPaused(
                                        JNIEnv *env, jobject thiz, jobject activity) {
        gpg::AndroidSupport::OnActivityPaused(env,activity);
    }
    
    virtual void nativeOnActivityResumed(
                                         JNIEnv *env, jobject thiz, jobject activity) {
        gpg::AndroidSupport::OnActivityResumed(env,activity);
    }
    
    virtual void nativeOnActivitySaveInstanceState(
                                                   JNIEnv *env, jobject thiz, jobject activity, jobject out_state) {
        gpg::AndroidSupport::OnActivitySaveInstanceState(env,activity,out_state);
    }
    
    virtual void nativeOnActivityStarted(
                                         JNIEnv *env, jobject thiz, jobject activity) {
        gpg::AndroidSupport::OnActivityStarted(env,activity);
    }
    
    virtual void nativeOnActivityStopped(
                                         JNIEnv *env, jobject thiz, jobject activity) {
        gpg::AndroidSupport::OnActivityStopped(env,activity);
    }
    
    virtual bool nativeOnActivityResult(
                                        JNIEnv *env,
                                        jobject thiz,
                                        jobject activity,
                                        jint request_code,
                                        jint result_code,
                                        jobject data) {
        gpg::AndroidSupport::OnActivityResult(env,activity,request_code,result_code,data);
        return false;
    }
} gps_extension;