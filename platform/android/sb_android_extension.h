#ifndef SB_ANDROID_EXTENSION_H_INCLUDED
#define SB_ANDROID_EXTENSION_H_INCLUDED

#include <sb_extension.h>
#include <jni.h>
#include <android/native_activity.h>

namespace Sandbox {
    
    class Application;

    class AndroidPlatformExtension : public PlatformExtension {
    private:
        AndroidPlatformExtension* m_next;
    protected:
        ANativeActivity* GetNativeActivity(Application* app);
    public:
        AndroidPlatformExtension();
        ~AndroidPlatformExtension();
        
        AndroidPlatformExtension* GetNext() { return m_next; }
        
        virtual void nativeOnActivityCreated(
                                                                          JNIEnv *env,
                                                                          jobject thiz,
                                                                          jobject activity,
                                                                          jobject saved_instance_state) {
        }
        
        virtual void nativeOnActivityDestroyed(
                                                                            JNIEnv *env, jobject thiz, jobject activity) {
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
          return false;
        }
    };
    
}

#endif /*SB_ANDROID_EXTENSION_H_INCLUDED*/