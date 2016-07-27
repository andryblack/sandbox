#include "sb_android_extension.h"
#include <sb_application.h>
#include <ghl_system.h>

namespace Sandbox {
    
    static AndroidPlatformExtension*& root() {
        static AndroidPlatformExtension* r = 0;
        return r;
    }

    ANativeActivity* AndroidPlatformExtension::GetNativeActivity(Application* app) {
        if (!app)
            return 0;
        GHL::System* sys = app->GetSystem();
        if (!sys)
            return 0;
        ANativeActivity* activity = 0;
        if (sys->GetDeviceData(GHL::DEVICE_DATA_APPLICATION,&activity)) {
            return activity;
        }
        return 0;
    }
    
    AndroidPlatformExtension::AndroidPlatformExtension() : m_next(root()) {
        root() = this;
    }
    AndroidPlatformExtension::~AndroidPlatformExtension() {
        
    }
    
   
}


extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityCreated(
                                                                  JNIEnv *env,
                                                                  jobject thiz,
                                                                  jobject activity,
                                                                  jobject saved_instance_state) {
    Sandbox::AndroidPlatformExtension* ext = Sandbox::root();
    while (ext) {
        ext->nativeOnActivityCreated(
                                     env,
                                     thiz,
                                     activity,
                                     saved_instance_state);
        ext = ext->GetNext();
    }
}
extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityDestroyed(
                                                                    JNIEnv *env, jobject thiz, jobject activity) {
    Sandbox::AndroidPlatformExtension* ext = Sandbox::root();
    while (ext) {
        ext->nativeOnActivityDestroyed(
                                     env,
                                     thiz,
                                     activity);
        ext = ext->GetNext();
    }
}
extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityPaused(
                                                                 JNIEnv *env, jobject thiz, jobject activity) {
    Sandbox::AndroidPlatformExtension* ext = Sandbox::root();
    while (ext) {
        ext->nativeOnActivityPaused(
                                       env,
                                       thiz,
                                       activity);
        ext = ext->GetNext();
    }
}
extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityResumed(
                                                                  JNIEnv *env, jobject thiz, jobject activity) {
    Sandbox::AndroidPlatformExtension* ext = Sandbox::root();
    while (ext) {
        ext->nativeOnActivityResumed(
                                    env,
                                    thiz,
                                    activity);
        ext = ext->GetNext();
    }
}
extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivitySaveInstanceState(
                                                                            JNIEnv *env, jobject thiz, jobject activity, jobject out_state) {
    Sandbox::AndroidPlatformExtension* ext = Sandbox::root();
    while (ext) {
        ext->nativeOnActivitySaveInstanceState(
                                     env,
                                     thiz,
                                     activity,out_state);
        ext = ext->GetNext();
    }
}
extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityStarted(
                                                                  JNIEnv *env, jobject thiz, jobject activity) {
    Sandbox::AndroidPlatformExtension* ext = Sandbox::root();
    while (ext) {
        ext->nativeOnActivityStarted(
                                     env,
                                     thiz,
                                     activity);
        ext = ext->GetNext();
    }
}
extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityStopped(
                                                                  JNIEnv *env, jobject thiz, jobject activity) {
    Sandbox::AndroidPlatformExtension* ext = Sandbox::root();
    while (ext) {
        ext->nativeOnActivityStopped(
                                     env,
                                     thiz,
                                     activity);
        ext = ext->GetNext();
    }
}
extern "C" JNIEXPORT
jboolean JNICALL Java_com_sandbox_Activity_nativeOnActivityResult(
                                                                 JNIEnv *env,
                                                                 jobject thiz,
                                                                 jobject activity,
                                                                 jint request_code,
                                                                 jint result_code,
                                                                 jobject data) {
    Sandbox::AndroidPlatformExtension* ext = Sandbox::root();
    while (ext) {
        if (ext->nativeOnActivityResult(
                                     env,
                                     thiz,
                                     activity,request_code,result_code,data)) {
            return JNI_TRUE;
        }
        ext = ext->GetNext();
    }
    return JNI_FALSE;
}
