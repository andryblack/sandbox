#include <sb_log.h>
#include <sb_application.h>
#include <ghl_system.h>
#include <json/sb_json.h>
#include <sbstd/sb_map.h>


#include "jni_utils.h"

#include <sbstd/sb_assert.h>

static const char* MODULE = "IAP";

IAPExtension::IAPExtension() : m_application(0),m_activity(0),m_helper_objl(0) {
	m_helper_ctr = 0;
    set_helper_class("com/sandbox/IAPHelper");
    SB_LOGI("IAPExtension::IAPExtension");
}
IAPExtension::~IAPExtension() {
	SB_LOGI("IAPExtension::~IAPExtension");
}
void IAPExtension::set_helper_class(const char* str) {
    sb_assert(m_helper_ctr == 0);
    m_helper_class = str;
}
void IAPExtension::release_object(JNIEnv* env) {
	if (m_helper_objl) {
		if (m_helper_dispose) {
			env->CallVoidMethod(m_helper_objl,m_helper_dispose);
		}
		env->DeleteGlobalRef(m_helper_objl);
		m_helper_objl = 0;
	}
}
void IAPExtension::resolve_methods(JNIEnv *env) {
	if (m_helper_ctr)
		return;
    sb_assert(!m_helper_class.empty());
    jni::check_exception(env);
	jni::class_ref_hold IAPHelper(env);
	IAPHelper.clazz = env->FindClass(m_helper_class.c_str());
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
    
void IAPExtension::OnAppStarted(Sandbox::Application* app) {
    m_application = app;
    m_activity = GetNativeActivity(m_application);
}
void IAPExtension::OnAppStopped(Sandbox::Application* app) {
    m_application = 0;
    m_activity = 0;
}
 bool IAPExtension::Process(Sandbox::Application* app,
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
    
void IAPExtension::processResponse(const sb::string& method, const sb::string& data) {
	if (m_application) {
		m_application->OnExtensionResponse(method.c_str(),data.c_str());
	} else {
        SB_LOGE("empty application");
    }
}
    
void IAPExtension::nativeOnActivityCreated(
                                     JNIEnv *env,
                                     jobject thiz,
                                     jobject activity,
                                     jobject saved_instance_state) {
    Sandbox::LogInfo() << "IAPExtension::nativeOnActivityCreated";
    resolve_methods(env);
    sb_assert(!m_helper_class.empty());
	jni::class_ref_hold IAPHelper(env);
	IAPHelper.clazz = env->FindClass(m_helper_class.c_str());
	jobject loc = env->NewObject(IAPHelper.clazz,m_helper_ctr,activity,(jlong)this);
	m_helper_objl = env->NewGlobalRef(loc);
	env->DeleteLocalRef(loc);
}
    
void IAPExtension::nativeOnActivityDestroyed(
                                       JNIEnv *env, jobject thiz, jobject activity) {
	release_object(env);
}
    
void IAPExtension::nativeOnActivityPaused(
                                    JNIEnv *env, jobject thiz, jobject activity) {
}
    
void IAPExtension::nativeOnActivityResumed(
                                     JNIEnv *env, jobject thiz, jobject activity) {
}
    
void IAPExtension::nativeOnActivitySaveInstanceState(
                                               JNIEnv *env, jobject thiz, jobject activity, jobject out_state) {
}
    
void IAPExtension::nativeOnActivityStarted(
                                     JNIEnv *env, jobject thiz, jobject activity) {
}
    
void IAPExtension::nativeOnActivityStopped(
                                     JNIEnv *env, jobject thiz, jobject activity) {
}
    
 bool IAPExtension::nativeOnActivityResult(
                                    JNIEnv *env,
                                    jobject thiz,
                                    jobject activity,
                                    jint request_code,
                                    jint result_code,
                                    jobject data) {
	if (m_helper_objl) {
		return m_activity->env->CallBooleanMethod(m_helper_objl,
            m_helper_process_result,
            request_code,
            result_code,data);
	}
	return false;
}

extern "C" void *init_iap_extension();

extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_IAPHelper_nativeProcessResponse(
                                                                 JNIEnv *env,
                                                                 jclass clazz,
                                                                 jlong object,
                                                                 jobject method,
                                                                 jobject data) {
    SB_LOGI("IAPHelper::nativeProcessResponse");
    sb::string str_method = jni::extract_jni_string(env,(jstring)method);
    sb::string str_data = jni::extract_jni_string(env,(jstring)data);
    static_cast<IAPExtension*>(init_iap_extension())->processResponse(str_method,str_data);
}

extern "C" void *init_iap_extension() {
    static IAPExtension __iap_extension;
    return &__iap_extension;
}
