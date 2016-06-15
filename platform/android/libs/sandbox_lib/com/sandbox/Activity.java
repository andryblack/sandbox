package com.sandbox;
import android.content.Intent;
import android.os.Bundle;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;

public class Activity  extends com.GHL.Activity {
    

    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        nativeOnActivityCreated(this, savedInstanceState);
    }
    
    protected void onDestroy(){
        super.onDestroy();
        nativeOnActivityDestroyed(this);
    }
    
    protected void onPause(){
        super.onPause();
        nativeOnActivityPaused(this);
    }
    
    protected void onResume(){
        super.onResume();
        nativeOnActivityResumed(this);
    }
    
    protected void onStart(){
        super.onStart();
        nativeOnActivityStarted(this);
    }
    
    protected void onStop(){
        super.onStop();
        nativeOnActivityStopped(this);
    }
    
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        nativeOnActivitySaveInstanceState(this, outState);
    }
    
    protected void onActivityResult(int requestCode,
                                        int resultCode,
                                        Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
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