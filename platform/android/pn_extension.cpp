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
	
public:
	PNExtension() : m_application(0),m_activity(0) {
		GHL_Log(GHL::LOG_LEVEL_INFO,"PNExtension::PNExtension\n");
	}
	~PNExtension() {
		GHL_Log(GHL::LOG_LEVEL_INFO,"PNExtension::~PNExtension\n");
	}
    
    virtual void OnAppStarted(Sandbox::Application* app) {
        m_application = app;
        m_activity = GetNativeActivity(m_application);
    }
    sb::string get_pn_token() {
        JNIEnv* env = m_activity->env;
        jni::class_ref_hold FirebaseInstanceId(env);
        FirebaseInstanceId.clazz = env->FindClass("com/google/firebase/iid/FirebaseInstanceId");
        if (!FirebaseInstanceId.clazz || jni::check_exception(env)) {
            Sandbox::LogError() << "PNExtension not found class";
            return failed_get;
        }

        jmethodID getInstance = env->GetStaticMethodID(FirebaseInstanceId.clazz,"getInstance","()Lcom/google/firebase/iid/FirebaseInstanceId;");
        if (!getInstance || jni::check_exception(env)) {
            Sandbox::LogError() << "PNExtension not found getInstance method";
            return failed_get;
        }
        jobject obj = env->CallStaticObjectMethod(FirebaseInstanceId.clazz,getInstance);
        if (!obj || jni::check_exception(env)) {
            Sandbox::LogError() << "PNExtension not found instance obj";
            return failed_get;
        }
        jmethodID getToken = env->GetMethodID(FirebaseInstanceId.clazz,"getToken","()Ljava/lang/String;");
        if (!getToken || jni::check_exception(env)) {
            env->DeleteLocalRef(obj);
            Sandbox::LogError() << "PNExtension not found getToken method";
            return failed_get;
        }
        jstring token_jni = (jstring)env->CallObjectMethod(obj,getToken);
        if (token_jni == 0 || jni::check_exception(env)) {
            return pending_get;
        }
        sb::string res = Sandbox::JsonBuilder()
            .BeginObject()
                .Key("status").PutString("success")
                .Key("token").PutString(jni::extract_jni_string(env,token_jni).c_str())
            .EndObject()
        .End();
        return res;
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
            sb::string res = Sandbox::JsonBuilder()
                .BeginObject()
                    .Key("status").PutString("success")
                    .Key("token").PutString(token.c_str())
                .EndObject()
            .End();
            m_application->OnExtensionResponse("PNGetToken", res.c_str());
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
