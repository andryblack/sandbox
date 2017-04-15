

#include <sb_extension.h>
#include <sb_log.h>
#include <sb_application.h>
#include <ghl_system.h>
#include <json/sb_json.h>
#include <sbstd/sb_map.h>

#include "sb_android_extension.h"
#include "jni_utils.h"

#include <android/native_activity.h>

static const char* MODULE = "GPGS";





class GPSExtension : public Sandbox::AndroidPlatformExtension {
    Sandbox::Application*   m_application;
    ANativeActivity*        m_activity;
    jmethodID   m_helper_ctr;
    jmethodID   m_helper_is_vailable;
    jmethodID   m_helper_sign_in;
    jmethodID   m_helper_sign_out;
    jmethodID   m_helper_get_player;
    jmethodID   m_helper_stop;
    jmethodID   m_helper_process_result;
    jmethodID   m_helper_send_scores;
    jmethodID   m_helper_send_achievements;
    jmethodID   m_helper_get_friends;
    jmethodID   m_helper_show_ui;
    jobject     m_helper_obj;
public:
    

    GPSExtension() : m_application(0),m_activity(0),m_helper_obj(0) {}
    virtual void OnAppStarted(Sandbox::Application* app) {
        m_application = app;
        m_activity = GetNativeActivity(m_application);
    }

    void resolve_methods(JNIEnv *env) {
        if (m_helper_ctr)
            return;
        jni::check_exception(env);
        jni::class_ref_hold GPGHelper(env);
        GPGHelper.clazz = env->FindClass("com/sandbox/GPSHelper");
        jni::check_exception(env);
        sb_assert(GPGHelper.clazz);
        m_helper_ctr = env->GetMethodID(GPGHelper.clazz,"<init>","(Lcom/sandbox/Activity;)V");
        jni::check_exception(env);
        sb_assert(m_helper_ctr);
        m_helper_process_result = env->GetMethodID(GPGHelper.clazz,"handleActivityResult","(IILandroid/content/Intent;)Z");
        jni::check_exception(env);
        sb_assert(m_helper_process_result);

        m_helper_is_vailable = env->GetMethodID(GPGHelper.clazz,"is_available","()Z");
        jni::check_exception(env);
        sb_assert(m_helper_is_vailable);
       
        m_helper_sign_in = env->GetMethodID(GPGHelper.clazz,"sign_in","(Z)Z");
        jni::check_exception(env);
        sb_assert(m_helper_sign_in);
        m_helper_get_player = env->GetMethodID(GPGHelper.clazz,"get_player","()Ljava/lang/String;");
        jni::check_exception(env);
        sb_assert(m_helper_get_player);
        m_helper_sign_out = env->GetMethodID(GPGHelper.clazz,"sign_out","()V");
        jni::check_exception(env);
        sb_assert(m_helper_sign_out);
        m_helper_stop = env->GetMethodID(GPGHelper.clazz,"stop","()V");
        jni::check_exception(env);
        sb_assert(m_helper_stop);

        m_helper_send_scores = env->GetMethodID(GPGHelper.clazz,"send_scores","(Ljava/lang/String;)Z");
        jni::check_exception(env);
        sb_assert(m_helper_send_scores);
        m_helper_send_achievements = env->GetMethodID(GPGHelper.clazz,"send_achievements","(Ljava/lang/String;)Z");
        jni::check_exception(env);
        sb_assert(m_helper_send_achievements);
        m_helper_get_friends = env->GetMethodID(GPGHelper.clazz,"get_friends","()Z");
        jni::check_exception(env);
        sb_assert(m_helper_get_friends);
        m_helper_show_ui = env->GetMethodID(GPGHelper.clazz,"show_ui","(Ljava/lang/String;)V");
        jni::check_exception(env);
        sb_assert(m_helper_show_ui);
     }

    bool sign_in(bool use_ui) {
        if (m_helper_obj) {
            JNIEnv* env = m_activity->env;
            return env->CallBooleanMethod(m_helper_obj,m_helper_sign_in,use_ui ? JNI_TRUE : JNI_FALSE);
        }
        return false;
    }
    void logout() {
        if (m_helper_obj) {
            JNIEnv* env = m_activity->env;
            env->CallVoidMethod(m_helper_obj,m_helper_sign_out);
        }
    }
    sb::string get_player() {
        if (m_helper_obj) {
            JNIEnv* env = m_activity->env;
            jobject obj = env->CallObjectMethod(m_helper_obj,m_helper_get_player);
            if (obj) {
                sb::string res = jni::extract_jni_string(env,(jstring)obj);
                env->DeleteLocalRef(obj);
                return res;
            }
        }
        return ""; 
    }

    bool send_scores(const char* data) {
        if (!m_helper_obj)
            return false;
        JNIEnv* env = m_activity->env;
        jni::jni_string str(data,env);
        return env->CallBooleanMethod(m_helper_obj,m_helper_send_scores,str.jstr);
    }
    bool send_achievements(const char* data) {
        if (!m_helper_obj)
            return false;
        JNIEnv* env = m_activity->env;
        jni::jni_string str(data,env);
        return env->CallBooleanMethod(m_helper_obj,m_helper_send_achievements,str.jstr);
    }

    bool get_friends() {
        if (!m_helper_obj)
            return false;
        JNIEnv* env = m_activity->env;
        return env->CallBooleanMethod(m_helper_obj,m_helper_get_friends);
    }
    bool show_ui(const char* name) {
        if (!m_helper_obj)
            return false;
        JNIEnv* env = m_activity->env;
        jni::jni_string namej(name,env);
        env->CallVoidMethod(m_helper_obj,m_helper_show_ui,namej.jstr);
        return true;
    }

    

    void processSignInResponse(const sb::string& data) {
        SB_LOGI("processSignInResponse: " << data);
        this->AddPendingResponse("GPSLogin",data.c_str());
    }
    void processGetFriendsResponse(const sb::string& data) {
        SB_LOGI("processGetFriendsResponse: " << data);
        this->AddPendingResponse("GPSGetFriends",data.c_str());
    }


    virtual bool Process(Sandbox::Application* app,
                         const char* method,
                         const char* args,
                         sb::string& res) {
        if (::strcmp("GPSLogin",method)==0) {
            if (!sign_in(::strcmp(args,"force_ui")==0)) {
                res = "failed";
            } else {
                res = "progress";
            }
            return true;
        }
       if (::strcmp("GPSGetPlayer",method)==0) {
            res = get_player();
            return true;
        }
        if (::strcmp("GPSSendScores",method)==0) {
            if (send_scores(args)) {
                res = "success";
            } else {
                res = "failed";
            }
            return true;
        }
        if (::strcmp("GPSSendAchievements",method)==0) {
            if (send_achievements(args)) {
                res = "success";
            } else {
                res = "failed";
            }
            return true;
        }
        if (::strcmp("GPSLogout",method)==0) {
            logout();
            return true;
        }
        if (::strcmp("GPSGetFriends",method)==0) {
            if (!get_friends()) {
                res = "failed";
            } else {
                res = "progress";
            }
            return true;
        }
        if (::strcmp("GPSShowUI",method)==0) {
            show_ui(args);
            return true;
        }
        return false;
    }

    void release_object() {
        if (m_helper_obj) {
            m_activity->env->DeleteGlobalRef(m_helper_obj);
            m_helper_obj = 0;
        }
    }
    
    
    virtual void nativeOnActivityCreated(
                                         JNIEnv *env,
                                         jobject thiz,
                                         jobject activity,
                                         jobject saved_instance_state) {
        
        resolve_methods(env);
        jni::class_ref_hold GPSHelper(env);
        GPSHelper.clazz = env->FindClass("com/sandbox/GPSHelper");
        jobject loc = env->NewObject(GPSHelper.clazz,m_helper_ctr,activity);
        m_helper_obj = env->NewGlobalRef(loc);
        env->DeleteLocalRef(loc);
    }
    
    virtual void nativeOnActivityDestroyed(
                                           JNIEnv *env, jobject thiz, jobject activity) {
        release_object();
    }
    
    virtual void nativeOnActivityPaused(
                                        JNIEnv *env, jobject thiz, jobject activity) {
    }
    
    virtual void nativeOnActivityResumed(
                                         JNIEnv *env, jobject thiz, jobject activity) {
    }
    
    virtual void nativeOnActivitySaveInstanceState(
                                                   JNIEnv *env, jobject thiz, jobject activity, jobject out_state) {
    }
    
    virtual void nativeOnActivityStarted(
                                         JNIEnv *env, jobject thiz, jobject activity) {
    }
    
    virtual void nativeOnActivityStopped(
                                         JNIEnv *env, jobject thiz, jobject activity) {
        if (m_helper_obj) {
            env->CallVoidMethod(m_helper_obj,m_helper_stop);
        }
    }

    virtual bool nativeOnActivityResult(
                                        JNIEnv *env,
                                        jobject thiz,
                                        jobject activity,
                                        jint request_code,
                                        jint result_code,
                                        jobject data) {
        if (m_helper_obj) {
            return m_activity->env->CallBooleanMethod(m_helper_obj,m_helper_process_result,request_code,result_code,data);
        }
        return false;
    }
    

};

// void GPSExtension::InitServices(gpg::PlatformConfiguration const &pc) {
//     SB_LOGI( "Initializing Services" );
//     if (!game_services_) {
//         SB_LOGI( "Uninitialized services, so creating" );
//         game_services_ = gpg::GameServices::Builder()
//         .SetOnLog([](gpg::LogLevel level, std::string const & msg) {
//             switch(level) {
//                 case gpg::LogLevel::INFO:
//                     SB_LOGI(msg);
//                     break;
//                 case gpg::LogLevel::WARNING:
//                     SB_LOGW(msg);
//                     break;
//                 case gpg::LogLevel::ERROR:
//                     SB_LOGE(msg);
//                     break;
//                 default:
//                     SB_LOGV(msg);
//                     break;
//             }
//         }, gpg::LogLevel::VERBOSE) 
//         .SetOnAuthActionStarted([this](gpg::AuthOperation op) {
//             SB_LOGI( "OnAuthActionStarted " << op );
//             if (op == gpg::AuthOperation::SIGN_IN) {
//                 is_auth_in_progress_ = "progress";
//                 this->AddPendingResponse("GPSLogin","progress");
//             } else if (op == gpg::AuthOperation::SIGN_OUT) {
//                 this->AddPendingResponse("GPSLogin","changed");
//             }
//         })
//         .SetOnAuthActionFinished([this](gpg::AuthOperation op,
//                                                      gpg::AuthStatus status) {
//             SB_LOGI( "OnAuthActionFinished" << op << " with a result of " << status);
//             if (op == gpg::AuthOperation::SIGN_IN) {
//                 if (gpg::IsSuccess(status)) {
//                     game_services_->Players().FetchSelf([this](const gpg::PlayerManager::FetchSelfResponse& resp) {
//                         if (gpg::IsSuccess(resp.status)) {
//                             is_auth_in_progress_ = "success";
                            
//                             me_player_data = Sandbox::JsonBuilder()
//                                 .BeginObject()
//                                 .Key("id").PutString(resp.data.Id().c_str())
//                                 .Key("name").PutString(resp.data.Name().c_str())
//                                 .Key("avatar").PutString(resp.data.AvatarUrl(gpg::ImageResolution::ICON).c_str())
//                                 .EndObject()
//                             .End();
//                             this->AddPendingResponse("GPSLogin","success");
//                         } else {
//                             is_auth_in_progress_ = "failed";
//                             this->AddPendingResponse("GPSLogin","failed");
//                         }
//                         game_services_->Leaderboards().FetchAll(nullptr);
//                     });
//                 } else {
//                     is_auth_in_progress_ = "failed";
//                     this->AddPendingResponse("GPSLogin","failed");
//                 }
//             } else if (op == gpg::AuthOperation::SIGN_OUT) {
//                 this->AddPendingResponse("GPSLogin","changed");
//             }
            
            
// //            Sandbox::LogInfo() <<"Fetching all nonblocking";
// //            game_services_->Achievements().FetchAll(gpg::DataSource::CACHE_OR_NETWORK, [] (gpg::AchievementManager::FetchAllResponse response) {
// //                    Sandbox::LogInfo() << "Achievement response status: " << response.status);
// //                });
// //            Sandbox::LogInfo() <<"--------------------------------------------------------------";
            
//         })
//         .Create(pc);
//     }
//     SB_LOGI("Created");
// }

extern "C" void *init_gps_extension() {
    static GPSExtension __gps_extension;
    return &__gps_extension;
}


extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_GPSHelper_nativeSignInResponse(
                                                                 JNIEnv *env,
                                                                 jclass clazz,
                                                                 jobject data) {
    GHL_Log(GHL::LOG_LEVEL_INFO,"Java_com_sandbox_GPSHelper_nativeSignInResponse\n");
    sb::string str_data = jni::extract_jni_string(env,(jstring)data);
    static_cast<GPSExtension*>(init_gps_extension())->processSignInResponse(str_data);
}

extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_GPSHelper_nativeGetFriendsResponse(
                                                                 JNIEnv *env,
                                                                 jclass clazz,
                                                                 jobject data) {
    GHL_Log(GHL::LOG_LEVEL_INFO,"Java_com_sandbox_GPSHelper_nativeGetFriendsResponse\n");
    sb::string str_data = jni::extract_jni_string(env,(jstring)data);
    static_cast<GPSExtension*>(init_gps_extension())->processGetFriendsResponse(str_data);
}

// sb::string GPSExtension::BeginGetFriends() {
//     if (!game_services_) {
//         SB_LOGE("BeginGetFriends error");
//         return "failed";
//     }
//     if (!game_services_->IsAuthorized()) {
//         return "failed";
//     } 
//     game_services_->Players().FetchConnected([](const gpg::PlayerManager::FetchListResponse& resp){
//         if (!gpg::IsSuccess(resp.status)) {
//             static_cast<GPSExtension*>(init_gps_extension())->AddPendingResponse("GPSGetFriends","failed");
//         } else {
//             Sandbox::JsonBuilder res;
//             res.BeginArray();
//             for (std::vector< gpg::Player >::const_iterator it = resp.data.begin(); it!=resp.data.end(); ++it) {
//                 res.BeginObject();
//                 res.Key("id").PutString(it->Id().c_str());
//                 res.Key("name").PutString(it->Name().c_str());
//                 res.Key("avatar").PutString(it->AvatarUrl(gpg::ImageResolution::ICON).c_str());
//                 res.EndObject();
//             }
//             res.EndArray();
//             static_cast<GPSExtension*>(init_gps_extension())->AddPendingResponse("GPSGetFriends",res.End().c_str());
//         }
//     });
//     return "pending";
// }

