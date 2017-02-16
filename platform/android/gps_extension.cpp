

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
#include "gpg/achievement_manager.h"

#include <android/native_activity.h>

static sb::string is_auth_in_progress_;
static std::unique_ptr<gpg::GameServices> game_services_;
static sb::string me_player_data;

sb::string BeginUserInitiatedSignIn() {
    if (!game_services_) {
        Sandbox::LogError() << "[PGS]" << "BeginUserInitiatedSignIn : game_services_";
        return "uninitialized";
    }
    if (!game_services_->IsAuthorized()) {
        Sandbox::LogInfo() << "[PGS]" << "BeginUserInitiatedSignIn: StartAuthorizationUI";
        game_services_->StartAuthorizationUI();
        return "pending";
    }
    return "success";
}

void BeginUserInitiatedSignOut() {
    if (!game_services_) {
        return;
    }
    if (!game_services_->IsAuthorized())
        return;
    is_auth_in_progress_ = "logout";
    game_services_->SignOut();
}


void SubmitHighScore(const sb::string& leaderboard_id, uint64_t score) {
    if (!game_services_)
        return;
    if (game_services_->IsAuthorized()) {
        Sandbox::LogInfo() << "[PGS]" << "High score submitted " << score << " " << leaderboard_id;
        game_services_->Leaderboards().SubmitScore(leaderboard_id, score);
    } else {
        //BeginUserInitiatedSignIn();
    }
}

void UnlockAchievement(const sb::string& achivement_id) {
    if (!game_services_)
        return;
    if (game_services_->IsAuthorized()) {
        Sandbox::LogInfo() << "[PGS]" << "Achievement unlocked " << achivement_id;
        game_services_->Achievements().Unlock(achivement_id);
    } else {
        //BeginUserInitiatedSignIn();
    }
}

sb::string ShowScoresUI() {
    if (!game_services_) {
        Sandbox::LogError() << "[PGS]" << "ShowScoresUI error";
        return "error";
    }
    if (game_services_->IsAuthorized()) {
        Sandbox::LogInfo() << "[PGS]" << "High score show";
        //game_services_->Leaderboards().FetchAllBlocking(gpg::DataSource::CACHE_OR_NETWORK);
        game_services_->Leaderboards().ShowAllUI(nullptr);
        return "success";
    } else {
        BeginUserInitiatedSignIn();
    }
    return "error";
}

sb::string ShowAchievementsUI() {
    if (!game_services_) {
        Sandbox::LogError() << "[PGS]" << "ShowAchievementsUI error";
        return "error";
    }
    if (game_services_->IsAuthorized()) {
        Sandbox::LogInfo() << "[PGS]" << "Achievements show";
        //game_services_->Leaderboards().FetchAllBlocking(gpg::DataSource::CACHE_OR_NETWORK);
        game_services_->Achievements().ShowAllUI(nullptr);
        return "success";
    } else {
        BeginUserInitiatedSignIn();
    }
    return "error";
}






class GPSExtension : public Sandbox::AndroidPlatformExtension {
    Sandbox::Application*   m_app;
public:
    void InitServices(gpg::PlatformConfiguration const &pc);

    GPSExtension() : m_app(0) {}
    virtual void OnAppStarted(Sandbox::Application* app) {
        m_app = app;
        ANativeActivity* activity = GetNativeActivity(m_app);
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

    struct ScoresSubmitter : public Sandbox::JsonTraverser {
        sb::string leaderboard_id;
        virtual void OnKey(const sb::string& v) {
            leaderboard_id = v;
        }
        virtual void OnInteger(int v) {
            if (!leaderboard_id.empty()) {
                SubmitHighScore(leaderboard_id,v);
            }
        }
        virtual void OnNumber(double v) {
            if (!leaderboard_id.empty()) {
                SubmitHighScore(leaderboard_id,v);
            }
        }

    };

    struct AchievementsSubmitter : public Sandbox::JsonTraverser {
        sb::string achivement_id;
        virtual void OnKey(const sb::string& v) {
            achivement_id = v;
        }
        virtual void OnInteger(int v) {
            if (!achivement_id.empty()) {
                if (v >= 100)
                    UnlockAchievement(achivement_id);
            }
        }
        virtual void OnNumber(double v) {
            if (!achivement_id.empty()) {
                if (v >= 100.0)
                    UnlockAchievement(achivement_id);
            }
        }

    };

    virtual bool Process(Sandbox::Application* app,
                         const char* method,
                         const char* args,
                         sb::string& res) {
        if (::strcmp("GPSSendScores",method)==0) {
            if (!game_services_->IsAuthorized()) {
                res = "failed";
                return true;
            }
            ScoresSubmitter submit;
            submit.TraverseString(args);
            res = "success";
            return true;
        }
        if (::strcmp("GPSSendAchievements",method)==0) {
            if (!game_services_->IsAuthorized()) {
                res = "failed";
                return true;
            }
            AchievementsSubmitter submit;
            submit.TraverseString(args);
            res = "success";
            return true;
        }
        if (::strcmp("GPSLogin",method)==0) {
            if (is_auth_in_progress_ == "progress" && !(args && ::strcmp(args,"force_ui")==0)) {
                res = is_auth_in_progress_;
                return true;
            }

            res = BeginUserInitiatedSignIn();
            return true;
        }
        if (::strcmp("GPSLogout",method)==0) {
           BeginUserInitiatedSignOut();
            return true;
        }
        if (::strcmp("GPSGetPlayer",method)==0) {
            if (!game_services_)
                return true;
            if (!game_services_->IsAuthorized())
                return true;
            if (is_auth_in_progress_=="logout")
                return true;
            res = me_player_data;
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
        if (::strcmp("GPSShowUI",method)==0) {
            if (::strcmp("leaderboards",args)==0) {
                res = ShowScoresUI();
            } else if (::strcmp("achievements",args)==0) {
                res = ShowAchievementsUI();
            }
            return true;
        }
        if (::strcmp("GPSGetFriends",method)==0) {
            res = BeginGetFriends();
            return true;
        }

        return false;
    }
    
    
    virtual void nativeOnActivityCreated(
                                         JNIEnv *env,
                                         jobject thiz,
                                         jobject aactivity,
                                         jobject saved_instance_state) {
        
        gpg::AndroidSupport::OnActivityCreated(env,aactivity,saved_instance_state);
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

    static sb::string BeginGetFriends();
};

void GPSExtension::InitServices(gpg::PlatformConfiguration const &pc) {
    Sandbox::LogInfo() << "[PGS]" << "Initializing Services";
    if (!game_services_) {
        Sandbox::LogInfo() << "[PGS]" << "Uninitialized services, so creating";
        game_services_ = gpg::GameServices::Builder()
        .SetOnLog(gpg::DEFAULT_ON_LOG, gpg::LogLevel::VERBOSE)
        .SetOnAuthActionStarted([this](gpg::AuthOperation op) {
            Sandbox::LogInfo() << "[PGS]" << "OnAuthActionStarted " << op;
            if (op == gpg::AuthOperation::SIGN_IN) {
                is_auth_in_progress_ = "progress";
                this->AddPendingResponse("GPSLogin","progress");
            } else if (op == gpg::AuthOperation::SIGN_OUT) {
                this->AddPendingResponse("GPSLogin","changed");
            }
        })
        .SetOnAuthActionFinished([this](gpg::AuthOperation op,
                                                     gpg::AuthStatus status) {
            Sandbox::LogInfo() << "[PGS]" << "OnAuthActionFinished" << op << " with a result of " << status;
            if (op == gpg::AuthOperation::SIGN_IN) {
                if (gpg::IsSuccess(status)) {
                    game_services_->Players().FetchSelf([this](const gpg::PlayerManager::FetchSelfResponse& resp) {
                        if (gpg::IsSuccess(resp.status)) {
                            is_auth_in_progress_ = "success";
                            
                            me_player_data = Sandbox::JsonBuilder()
                                .BeginObject()
                                .Key("id").PutString(resp.data.Id().c_str())
                                .Key("name").PutString(resp.data.Name().c_str())
                                .Key("avatar").PutString(resp.data.AvatarUrl(gpg::ImageResolution::ICON).c_str())
                                .EndObject()
                            .End();
                            this->AddPendingResponse("GPSLogin","success");
                        } else {
                            is_auth_in_progress_ = "failed";
                            this->AddPendingResponse("GPSLogin","failed");
                        }
                        game_services_->Leaderboards().FetchAll(nullptr);
                    });
                } else {
                    is_auth_in_progress_ = "failed";
                    this->AddPendingResponse("GPSLogin","failed");
                }
            } else if (op == gpg::AuthOperation::SIGN_OUT) {
                this->AddPendingResponse("GPSLogin","changed");
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

extern "C" void *init_gps_extension() {
    static GPSExtension __gps_extension;
    return &__gps_extension;
}

sb::string GPSExtension::BeginGetFriends() {
    if (!game_services_) {
        Sandbox::LogError() << "[PGS]" << "BeginGetFriends error";
        return "failed";
    }
    if (!game_services_->IsAuthorized()) {
        return "failed";
    } 
    game_services_->Players().FetchConnected([](const gpg::PlayerManager::FetchListResponse& resp){
        if (!gpg::IsSuccess(resp.status)) {
            static_cast<GPSExtension*>(init_gps_extension())->AddPendingResponse("GPSGetFriends","failed");
        } else {
            Sandbox::JsonBuilder res;
            res.BeginArray();
            for (std::vector< gpg::Player >::const_iterator it = resp.data.begin(); it!=resp.data.end(); ++it) {
                res.BeginObject();
                res.Key("id").PutString(it->Id().c_str());
                res.Key("name").PutString(it->Name().c_str());
                res.Key("avatar").PutString(it->AvatarUrl(gpg::ImageResolution::ICON).c_str());
                res.EndObject();
            }
            res.EndArray();
            static_cast<GPSExtension*>(init_gps_extension())->AddPendingResponse("GPSGetFriends",res.End().c_str());
        }
    });
    return "pending";
}

