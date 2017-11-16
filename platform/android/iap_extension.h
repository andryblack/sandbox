#ifndef _IAP_EXTENSION_H_INCLUDED_
#define _IAP_EXTENSION_H_INCLUDED_

#include "sb_android_extension.h"
#include <android/native_activity.h>

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

	sb::string m_helper_class;
public:
	IAPExtension();
	~IAPExtension();

	void set_helper_class(const char* str);

	void release_object(JNIEnv* env);
	void resolve_methods(JNIEnv *env);
    
    virtual void OnAppStarted(Sandbox::Application* app);
    virtual void OnAppStopped(Sandbox::Application* app);
    virtual bool Process(Sandbox::Application* app,
                         const char* method,
                         const char* args,
                         sb::string& res);
    
    void processResponse(const sb::string& method, const sb::string& data);
    
    virtual void nativeOnActivityCreated(
                                         JNIEnv *env,
                                         jobject thiz,
                                         jobject activity,
                                         jobject saved_instance_state);
    
    virtual void nativeOnActivityDestroyed(
                                           JNIEnv *env, jobject thiz, jobject activity) ;
    
    virtual void nativeOnActivityPaused(
                                        JNIEnv *env, jobject thiz, jobject activity) ;
    virtual void nativeOnActivityResumed(
                                         JNIEnv *env, jobject thiz, jobject activity) ;
    
    virtual void nativeOnActivitySaveInstanceState(
                                                   JNIEnv *env, jobject thiz, jobject activity, jobject out_state);
    
    virtual void nativeOnActivityStarted(
                                         JNIEnv *env, jobject thiz, jobject activity) ;
    
    virtual void nativeOnActivityStopped(
                                         JNIEnv *env, jobject thiz, jobject activity) ;
    
    virtual bool nativeOnActivityResult(
                                        JNIEnv *env,
                                        jobject thiz,
                                        jobject activity,
                                        jint request_code,
                                        jint result_code,
                                        jobject data) ;
};

extern "C" void *init_iap_extension();


#endif /*_IAP_EXTENSION_H_INCLUDED_*/