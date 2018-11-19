#ifndef _JNI_UTILS_H_INCLUDED_
#define _JNI_UTILS_H_INCLUDED_

#include <jni.h>
#include <sbstd/sb_string.h>

namespace Sandbox {
	class Application;
}

namespace jni {
	JNIEnv* get_env(Sandbox::Application* app);
	jobject get_activity(Sandbox::Application* app);

	struct class_ref_hold {
	    JNIEnv* env;
	    jclass clazz;
	    explicit class_ref_hold(JNIEnv* env) : env(env),clazz(0) {}
	    ~class_ref_hold() {
	        if (clazz)
	            env->DeleteLocalRef(clazz);
	    }
	};

	sb::string extract_jni_string( JNIEnv* env, const jstring jstr );
	sb::string jni_object_to_string(JNIEnv *env, jobject obj);
	bool check_exception( JNIEnv* env , bool clear = true);

	struct jni_string
    {
        jstring jstr;
        JNIEnv* env;
        explicit jni_string(const char* str, JNIEnv* env) : env(env) {
            jstr = convert_utf8(env,str);
        }
        explicit jni_string(jobject obj, JNIEnv* env) : jstr((jstring)obj),env(env){
        }
        ~jni_string() {
            if (jstr) {
                env->DeleteLocalRef( jstr );
            }
        }
        void assign(const char* str) {
            if (jstr) {
                env->DeleteLocalRef(jstr);
            }
            jstr = convert_utf8(env,str);
        }
        static jstring convert_utf8(JNIEnv* env,const char* str);
        static std::string extract( const jstring jstr , JNIEnv* env ) {
        	return extract_jni_string(env,jstr);
        }
        std::string str() { return extract(jstr,env);}
    };
}

#endif /*_JNI_UTILS_H_INCLUDED_*/