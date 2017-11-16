#include <sb_log.h>
#include <sb_application.h>
#include <ghl_system.h>
#include <json/sb_json.h>
#include <sbstd/sb_map.h>

#include "sb_android_extension.h"
#include "jni_utils.h"
#include <android/native_activity.h>
#include <sbstd/sb_assert.h>
#include <ghl_log.h>

static const char* failed_get = "{\"status\":\"failed\"}";
static const char* pending_get = "{\"status\":\"pending\"}";
class PNExtension : public Sandbox::AndroidPlatformExtension {
private:
	Sandbox::Application*	m_application;
	ANativeActivity* 		m_activity;
	jclass  m_SBFirebaseInstanceIDService;
public:
	PNExtension() : m_application(0),m_activity(0),m_SBFirebaseInstanceIDService(0) {
		GHL_Log(GHL::LOG_LEVEL_INFO,"PNExtension::PNExtension\n");
	}
	~PNExtension() {
		GHL_Log(GHL::LOG_LEVEL_INFO,"PNExtension::~PNExtension\n");
	}
    
    virtual void OnAppStarted(Sandbox::Application* app) {
        m_application = app;
        m_activity = GetNativeActivity(m_application);
    }
    virtual void OnAppStopped(Sandbox::Application* app){
        m_application = 0;
        m_activity = 0;
    }

    virtual void nativeOnActivityCreated(
                                         JNIEnv *env,
                                         jobject thiz,
                                         jobject activity,
                                         jobject saved_instance_state) {
        Sandbox::LogInfo() << "PNExtension::nativeOnActivityCreated";
        check_available(env,activity);
    }
    
    virtual void nativeOnActivityDestroyed(JNIEnv *env, jobject thiz, jobject activity) {
        if (m_SBFirebaseInstanceIDService) {
            env->DeleteGlobalRef(m_SBFirebaseInstanceIDService);
            m_SBFirebaseInstanceIDService = 0;
        }
    }   

    void check_available(JNIEnv* env,jobject activity) {
        m_SBFirebaseInstanceIDService = 0;
        jni::class_ref_hold SBFirebaseInstanceIDService(env);
        //com.google.android.gms.common.GoogleApiAvailability.getInstance();
        jni::check_exception(env);
        SBFirebaseInstanceIDService.clazz = env->FindClass("com/sandbox/SBFirebaseInstanceIDService");
        if (jni::check_exception(env) || !SBFirebaseInstanceIDService.clazz) {
            Sandbox::LogError() << "SBFirebaseInstanceIDService not found class";
            return;
        }
        jmethodID isGooglePlayServicesAvailable = env->GetStaticMethodID(SBFirebaseInstanceIDService.clazz,"isGooglePlayServicesAvailable","(Landroid/content/Context;)I");
        if (jni::check_exception(env) || !isGooglePlayServicesAvailable) {
            Sandbox::LogError() << "SBFirebaseInstanceIDService not found isGooglePlayServicesAvailable method";
            return;
        }
        int res = env->CallStaticIntMethod(SBFirebaseInstanceIDService.clazz,isGooglePlayServicesAvailable,activity);
        if (jni::check_exception(env)) {
            Sandbox::LogError() << "isGooglePlayServicesAvailable exception";
            return;
        }
        Sandbox::LogInfo() << "isGooglePlayServicesAvailable: " << res;
        if (res == 0) {
            m_SBFirebaseInstanceIDService =  (jclass)env->NewGlobalRef(SBFirebaseInstanceIDService.clazz);
        }
    }

    sb::string get_pn_token() {
        if (!m_SBFirebaseInstanceIDService || !m_activity || !m_activity->env) {
            return failed_get;
        }
        JNIEnv* env = m_activity->env;
        jni::check_exception(env);
        jmethodID getToken = env->GetStaticMethodID(m_SBFirebaseInstanceIDService,"getToken","()Ljava/lang/String;");
        if (jni::check_exception(env) || !getToken) {
            Sandbox::LogError() << "SBFirebaseInstanceIDService not found getToken method";
            return failed_get;
        }
      
        jstring token_jni = (jstring)env->CallStaticObjectMethod(m_SBFirebaseInstanceIDService,getToken);
        if (jni::check_exception(env) || token_jni == 0) {
            return pending_get;
        }
        Sandbox::JsonBuilder json;
        json.BeginObject()
                .Key("status").PutString("success")
                .Key("token").PutString(jni::extract_jni_string(env,token_jni).c_str())
            .EndObject();
        return json.End();
    }
    virtual bool Process(Sandbox::Application* app,
                         const char* method,
                         const char* args,
                         sb::string& res) {
    	if (::strcmp("PNGetToken",method) == 0) {
    		res = get_pn_token();
            return true;
    	} 
        return false;
    }
    
    void sendToken(const sb::string& token) {
    	if (m_application) {
            Sandbox::JsonBuilder json;
            json.BeginObject()
                    .Key("status").PutString("success")
                    .Key("token").PutString(token.c_str())
                .EndObject();
            m_application->OnExtensionResponse("PNGetToken", json.End().c_str());
    	}
    }
    
    
} ;

extern "C" void *init_pn_extension();

extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_SBFirebaseInstanceIDService_sendRegistrationToServer(
                                                                 JNIEnv *env,
                                                                 jclass clazz,
                                                                 jobject token) {
    GHL_Log(GHL::LOG_LEVEL_INFO,"Java_com_sandbox_SBFirebaseInstanceIDService_sendRegistrationToServer\n");
    sb::string str_token = jni::extract_jni_string(env,(jstring)token);
    static_cast<PNExtension*>(init_pn_extension())->sendToken(str_token);
}

extern "C" void *init_pn_extension() {
    static PNExtension __pn_extension;
    return &__pn_extension;
}
