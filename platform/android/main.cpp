#include <jni.h>

int StartApplication(int argc,char** argv);

volatile void prevent_strip_symbols();

int ghl_android_app_main(int argc,char** argv) {
	prevent_strip_symbols();
    StartApplication(argc,argv);
}

extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityCreated(
                                                                  JNIEnv *env,
                                                                  jobject thiz,
                                                                  jobject activity,
                                                                  jobject saved_instance_state);
extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityDestroyed(
                                                                    JNIEnv *env, jobject thiz, jobject activity);

extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityPaused(
                                                                 JNIEnv *env, jobject thiz, jobject activity);

extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityResumed(
                                                                  JNIEnv *env, jobject thiz, jobject activity);

extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivitySaveInstanceState(
                                                                            JNIEnv *env, jobject thiz, jobject activity, jobject out_state) ;
extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityStarted(
                                                                  JNIEnv *env, jobject thiz, jobject activity) ;
extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityStopped(
                                                                  JNIEnv *env, jobject thiz, jobject activity) ;
extern "C" JNIEXPORT
void JNICALL Java_com_sandbox_Activity_nativeOnActivityResult(
                                                                 JNIEnv *env,
                                                                 jobject thiz,
                                                                 jobject activity,
                                                                 jint request_code,
                                                                 jint result_code,
                                                                 jobject data) ;
void ensure_iap_extension_not_stripped();
volatile void* __prevent_strip_symbols;
volatile void prevent_strip_symbols() {
    __prevent_strip_symbols = (void*)&Java_com_sandbox_Activity_nativeOnActivityCreated;
    __prevent_strip_symbols = (void*)&Java_com_sandbox_Activity_nativeOnActivityDestroyed;
    __prevent_strip_symbols = (void*)&Java_com_sandbox_Activity_nativeOnActivityPaused;
    __prevent_strip_symbols = (void*)&Java_com_sandbox_Activity_nativeOnActivityResumed;
    __prevent_strip_symbols = (void*)&Java_com_sandbox_Activity_nativeOnActivitySaveInstanceState;
    __prevent_strip_symbols = (void*)&Java_com_sandbox_Activity_nativeOnActivityStopped;
    __prevent_strip_symbols = (void*)&Java_com_sandbox_Activity_nativeOnActivityResult;
    ensure_iap_extension_not_stripped();
}
