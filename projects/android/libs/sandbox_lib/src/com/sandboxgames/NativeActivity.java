package com.sandboxgames;
import android.content.Intent;
import android.os.Bundle;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;

public class NativeActivity  extends android.app.NativeActivity {
    
    private static boolean libloaded = false;
    private void ensureLoadLibrary() {
        if (libloaded) {
            return;
        }
        String libname = "main";
        try {
            ActivityInfo ai = getPackageManager().getActivityInfo(
                                                     getIntent().getComponent(), PackageManager.GET_META_DATA);
            if (ai.metaData != null) {
                String ln = ai.metaData.getString(android.app.NativeActivity.META_DATA_LIB_NAME);
                if (ln != null) libname = ln;
            }
        } catch (PackageManager.NameNotFoundException e) {
            throw new RuntimeException("Error getting activity info", e);
        }
        System.loadLibrary(libname);
        libloaded = true;
    }
    
    

    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        ensureLoadLibrary();
        nativeOnActivityCreated(this, savedInstanceState);
    }
    
    protected void onDestroy(){
        super.onDestroy();
        ensureLoadLibrary();
        nativeOnActivityDestroyed(this);
    }
    
    protected void onPause(){
        super.onPause();
        ensureLoadLibrary();
        nativeOnActivityPaused(this);
    }
    
    protected void onResume(){
        super.onResume();
        ensureLoadLibrary();
        nativeOnActivityResumed(this);
    }
    
    protected void onStart(){
        super.onStart();
        ensureLoadLibrary();
        nativeOnActivityStarted(this);
    }
    
    protected void onStop(){
        super.onStop();
        ensureLoadLibrary();
        nativeOnActivityStopped(this);
    }
    
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        ensureLoadLibrary();
        nativeOnActivitySaveInstanceState(this, outState);
    }
    
    protected void onActivityResult(int requestCode,
                                        int resultCode,
                                        Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        ensureLoadLibrary();
        nativeOnActivityResult(this, requestCode, resultCode, data);
    }
    
    // Implemented in C++.
    private static native void nativeOnActivityCreated(Activity activity, Bundle savedInstanceState);
    private static native void nativeOnActivityDestroyed(Activity activity);
    private static native void nativeOnActivityPaused(Activity activity);
    private static native void nativeOnActivityResumed(Activity activity);
    private static native void nativeOnActivitySaveInstanceState(Activity activity,
                                                                  Bundle outState);
    private static native void nativeOnActivityStarted(Activity activity);
    private static native void nativeOnActivityStopped(Activity activity);
    private static native void nativeOnActivityResult(Activity activity,
                                                            int requestCode,
                                                            int resultCode,
                                                            Intent data);

}