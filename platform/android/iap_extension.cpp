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

class IAPExtension : public Sandbox::AndroidPlatformExtension {
private:
	Sandbox::Application*	m_application;
	ANativeActivity* 		m_activity;
	jmethodID	m_helper_ctr;
	jmethodID	m_helper_dispose;
	jmethodID	m_helper_init;
	jmethodID	m_helper_get_products_info;
	jmethodID   m_helper_setup_done;
	jmethodID	m_helper_purchase;
	jmethodID	m_helper_process_result;
	jmethodID	m_helper_restore_payments;
	jmethodID	m_helper_confirm_transaction;
	jobject		m_helper_objl;

public:
	IAPExtension() : m_application(0),m_activity(0),m_helper_objl(0) {
		m_helper_ctr = 0;
		GHL_Log(GHL::LOG_LEVEL_INFO,"IAPExtension::IAPExtension\n");
	}
	~IAPExtension() {
		GHL_Log(GHL::LOG_LEVEL_INFO,"IAPExtension::~IAPExtension\n");
	}
	void release_object() {
		if (m_helper_objl) {
			if (m_helper_dispose) {
				m_activity->env->CallVoidMethod(m_helper_objl,m_helper_dispose);
			}
			m_activity->env->DeleteGlobalRef(m_helper_objl);
			m_helper_objl = 0;
		}
	}
	void resolve_methods(JNIEnv *env) {
		if (m_helper_ctr)
			return;
        jni::check_exception(env);
		jni::class_ref_hold IAPHelper(env);
		IAPHelper.clazz = env->FindClass("com/sandbox/IAPHelper");
		jni::check_exception(env);
		sb_assert(IAPHelper.clazz);
        m_helper_ctr = env->GetMethodID(IAPHelper.clazz,"<init>","(Lcom/sandbox/Activity;J)V");
		jni::check_exception(env);
		sb_assert(m_helper_ctr);
		m_helper_dispose = env->GetMethodID(IAPHelper.clazz,"dispose","()V");
		jni::check_exception(env);
		sb_assert(m_helper_dispose);
		m_helper_init = env->GetMethodID(IAPHelper.clazz,"iap_init","(Ljava/lang/String;)Z");
		jni::check_exception(env);
		sb_assert(m_helper_init);
		m_helper_get_products_info = env->GetMethodID(IAPHelper.clazz,"iap_get_products_info","(Ljava/lang/String;)Z");
		jni::check_exception(env);
		sb_assert(m_helper_get_products_info);
		m_helper_setup_done = env->GetMethodID(IAPHelper.clazz,"is_setup_done","()Z");
		jni::check_exception(env);
        sb_assert(m_helper_setup_done);
        m_helper_purchase = env->GetMethodID(IAPHelper.clazz,"iap_purchase","(Ljava/lang/String;Ljava/lang/String;)Z");
		jni::check_exception(env);
        sb_assert(m_helper_purchase);
        m_helper_process_result = env->GetMethodID(IAPHelper.clazz,"handleActivityResult","(IILandroid/content/Intent;)Z");
		jni::check_exception(env);
        sb_assert(m_helper_process_result);
        m_helper_restore_payments = env->GetMethodID(IAPHelper.clazz,"iap_restore_payments","()Z");
		jni::check_exception(env);
        sb_assert(m_helper_restore_payments);
       //boolean handleActivityResult(int requestCode, int resultCode, Intent data)
        m_helper_confirm_transaction = env->GetMethodID(IAPHelper.clazz,"iap_confirm_transaction","(Ljava/lang/String;)Z");
		jni::check_exception(env);
        sb_assert(m_helper_confirm_transaction);
	}
    
    virtual void OnAppStarted(Sandbox::Application* app) {
        m_application = app;
        m_activity = GetNativeActivity(m_application);
    }
    virtual bool Process(Sandbox::Application* app,
                         const char* method,
                         const char* args,
                         sb::string& res) {
    	if (::strcmp("iap_init",method) == 0) {
    		if (m_helper_objl) {
    			JNIEnv* env = m_activity->env;
    			jni::jni_string key(args,env);
    			if (env->CallBooleanMethod(m_helper_objl,m_helper_init,key.jstr)) {
    				res = env->CallBooleanMethod(m_helper_objl,m_helper_setup_done) ? "ready" : "pending";
    			} else {
    				res = "failed";
    			}
    			return true;
    		}
    	} else
        if (::strcmp("iap_get_products_info",method)==0) {
            if (m_helper_objl) {
            	JNIEnv* env = m_activity->env;
    			jni::jni_string key(args,env);
    			if (env->CallBooleanMethod(m_helper_objl,m_helper_get_products_info,key.jstr)) {
    				res = "pending";
    			} else {
    				res = "failed";
    			}
    			return true;
            }
        }
        if (::strcmp("iap_purchase",method)==0) {
            if (m_helper_objl) {
            	JNIEnv* env = m_activity->env;
    			jni::jni_string key(args,env);
    			jni::jni_string data("",env);
    			if (env->CallBooleanMethod(m_helper_objl,m_helper_purchase,key.jstr,data.jstr)) {
    				res = "pending";
    			} else {
    				res = "failed";
    			}
    			return true;
            }
        }
        if (::strcmp("iap_confirm_transaction",method)==0) {
            if (m_helper_objl) {
            	JNIEnv* env = m_activity->env;
            	jni::jni_string key(args,env);
    			if (env->CallBooleanMethod(m_helper_objl,m_helper_confirm_transaction,key.jstr)) {
    				res = "pending";
    			} else {
    				res = "failed";
    			}
    			return true;
            }
        }
        if (::strcmp("iap_restore_payments",method)==0) {
            if (m_helper_objl) {
            	JNIEnv* env = m_activity->env;
    			if (env->CallBooleanMethod(m_helper_objl,m_helper_restore_payments)) {
    				res = "pending";
    			} else {
    				res = "failed";
    			}
    			return true;
            }
        }

        return false;
    }
    
    void processResponse(const sb::string& method, const sb::string& data) {
    	if (m_application) {
    		m_application->OnExtensionResponse(method.c_str(),data.c_str());
    	}
    }
    
    virtual void nativeOnActivityCreated(
                                         JNIEnv *env,
                                         jobject thiz,
                                         jobject activity,
                                         jobject saved_instance_state) {
        Sandbox::LogInfo() << "IAPExtension::nativeOnActivityCreated";
        resolve_methods(env);
    	jni::class_ref_hold IAPHelper(env);
		IAPHelper.clazz = env->FindClass("com/sandbox/IAPHelper");
		jobject loc = env->NewObject(IAPHelper.clazz,m_helper_ctr,activity,(jlong)this);
		m_helper_objl = env->NewGlobalRef(loc);
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
    }
    
    virtual bool nativeOnActivityResult(
                                        JNIEnv *env,
                                        jobject thiz,
                                        jobject activity,
                                        jint request_code,
                                        jint result_code,
                                        jobject data) {
    	if (m_helper_objl) {
    		return m_activity->env->CallBooleanMethod(m_helper_objl,m_helper_process_result,request_code,result_code,data);
    	}
    	return false;
    }
} ;

extern "C" void *init_iap_extension();

extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_IAPHelper_nativeProcessResponse(
                                                                 JNIEnv *env,
                                                                 jclass clazz,
                                                                 jlong object,
                                                                 jobject method,
                                                                 jobject data) {
    GHL_Log(GHL::LOG_LEVEL_INFO,"Java_com_sandbox_IAPHelper_nativeProcessResponse\n");
    sb::string str_method = jni::extract_jni_string(env,(jstring)method);
    sb::string str_data = jni::extract_jni_string(env,(jstring)data);
    static_cast<IAPExtension*>(init_iap_extension())->processResponse(str_method,str_data);
}

extern "C" void *init_iap_extension() {
    static IAPExtension __iap_extension;
    return &__iap_extension;
}
