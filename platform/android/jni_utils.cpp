#include "jni_utils.h"
#include <ghl_system.h>
#include <sb_application.h>
#include <android/native_activity.h>
#include <sbstd/sb_string.h>

namespace jni {
	JNIEnv* get_env(Sandbox::Application* app) {
	    GHL::System* sys = app->GetSystem();
	    if (!sys) return 0;
	    ANativeActivity* activity = 0;
	    if (sys->GetDeviceData(GHL::DEVICE_DATA_APPLICATION,&activity)) {
	        return activity->env;
	    }
	    return 0;
	}

	jobject get_activity(Sandbox::Application* app) {
		GHL::System* sys = app->GetSystem();
	    if (!sys) return 0;
	    ANativeActivity* activity = 0;
	    if (sys->GetDeviceData(GHL::DEVICE_DATA_APPLICATION,&activity)) {
	        return activity->clazz;
	    }
	    return 0;
	}

	 sb::string
	extract_jni_string( JNIEnv* env, const jstring jstr ) {
	    if ( !env || !jstr ) {
	        return sb::string();
	    }
	    jsize size = env->GetStringUTFLength( jstr );
	    if( size == 0 ) {
	        return sb::string();;
	    }
	    jboolean is_copy = JNI_FALSE;
	    const char* psz  = env->GetStringUTFChars( jstr, &is_copy );
	    if ( is_copy == JNI_FALSE || !psz ) {
	        return sb::string();
	    }
	    
	    sb::string result( psz, size );
	    env->ReleaseStringUTFChars( jstr, psz );

	    return result;
	}


	sb::string jni_object_to_string(JNIEnv *env, jobject obj)
	{
	    jclass clazz = env->GetObjectClass(obj);
	    if (!clazz) return "not found class";
	    jmethodID getMessageID__ = env->GetMethodID(clazz, "toString", "()Ljava/lang/String;");
	    if(getMessageID__)
	    {
	        jstring jmsg__ = (jstring) env->CallObjectMethod(obj, getMessageID__);\
	        if (jmsg__) {
	           return extract_jni_string(env,jmsg__);
	        }
	        return "empty toString result";
	    }
	    return "not found toString";
	}

	bool check_exception( JNIEnv* env , bool clear ) {
        if (env->ExceptionCheck()) {
            jthrowable exc = env->ExceptionOccurred();
            if (exc && clear) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass clazz = env->GetObjectClass(exc);
                if (clazz) {
                    jmethodID toString = env->GetMethodID(clazz,"toString","()Ljava/lang/String;");
                    if (toString) {
                        jni_string str(env->CallObjectMethod(exc,toString),env);
                        Sandbox::LogError() << "[jni] exception: " << str.str();
                    } else {
                        env->FatalError("not fount toString for exception");
                    }
                    jmethodID printStackTrace = env->GetMethodID(clazz,"printStackTrace","()V");
                    if (printStackTrace) {
                        env->CallVoidMethod(exc,printStackTrace);
                    }
                } else {
                    env->FatalError("not fount class for exception");
                }
            }
            return true;
        }
        return false;
    }

}