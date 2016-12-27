/**
 * Copyright 2016 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.sandbox;

import android.util.Log;

import com.google.firebase.iid.FirebaseInstanceId;
import com.google.firebase.iid.FirebaseInstanceIdService;
import com.google.firebase.iid.FirebaseInstanceIdService;
import com.google.android.gms.common.GoogleApiAvailability;
import android.content.Context;

import org.json.*;

public class SBFirebaseInstanceIDService extends FirebaseInstanceIdService {

    private static final String TAG = "SBFirebaseIIDService";

    public static int isGooglePlayServicesAvailable(Context ctx) {
        return GoogleApiAvailability.getInstance().isGooglePlayServicesAvailable(ctx);
    }
    public static String getToken() {
        return parse_token(FirebaseInstanceId.getInstance().getToken());
    }
    /**
     * Called if InstanceID token is updated. This may occur if the security of
     * the previous token had been compromised. Note that this is called when the InstanceID token
     * is initially generated so this is where you would retrieve the token.
     */
    // [START refresh_token]
    @Override
    public void onTokenRefresh() {
        // Get updated InstanceID token.
        String refreshedToken = FirebaseInstanceId.getInstance().getToken();
        Log.d(TAG, "Refreshed token: " + refreshedToken);

        // If you want to send messages to this application instance or
        // manage this apps subscriptions on the server side, send the
        // Instance ID token to your app server.
        sendRegistrationToServer(parse_token(refreshedToken));
    }
    // [END refresh_token]

    private static String parse_token(String src) {
        if (src == null) {
            return null;
        }
        try {
            JSONObject jObject = new JSONObject(src);
            if (jObject.has("token")) {
                return jObject.getString("token");
            }
        } catch (JSONException e) {
            
        }
        return src;
    }

    /**
     * Persist token to third-party servers.
     *
     * Modify this method to associate the user's FCM InstanceID token with any server-side account
     * maintained by your application.
     *
     * @param token The new token.
     */
    private static native void sendRegistrationToServer(String token);
}
