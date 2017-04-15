

package com.sandbox;

import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.Player;
import com.google.android.gms.games.PlayerBuffer;
import com.google.android.gms.games.Players;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;

import android.content.IntentSender;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import java.util.Iterator;

import android.app.Dialog;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONException;

public class GPSHelper  
	implements GoogleApiClient.ConnectionCallbacks,
		GoogleApiClient.OnConnectionFailedListener,
		ResultCallback<Players.LoadPlayersResult> {

	private static final String TAG = "GPGS";
	private static final int RC_SIGN_IN = 9001;
	private static final int REQUEST_LEADERBOARD = 9002;
	private static final int REQUEST_ACHIEVEMENTS = 9003;

	private Activity m_activity;
	private GoogleApiClient m_google_api_client;
	// Are we currently resolving a connection failure?
  	private boolean m_resolving_connection_failure = false;
  	private boolean m_signin_called = false;
  	private boolean m_first_login_flow = true;

	public  GPSHelper(Activity activity) {
        m_activity = activity;
        m_google_api_client = new GoogleApiClient.Builder(m_activity)
	        .addConnectionCallbacks(this)
	        .addOnConnectionFailedListener(this)
	        .addApi(Games.API).addScope(Games.SCOPE_GAMES)
	        .build();
    }

	public boolean is_available() {
		return GoogleApiAvailability.getInstance().isGooglePlayServicesAvailable(m_activity) == ConnectionResult.SUCCESS;
	}

	public boolean sign_in(boolean use_ui) {
		if (!is_available()) {
			return false;
		}
		m_signin_called = use_ui;
		m_google_api_client.connect();
		return true;
	}

	public void sign_out() {
		if (m_google_api_client.isConnected()) {
			Games.signOut(m_google_api_client);
			m_google_api_client.disconnect();
			nativeSignInResponse("changed");
		}
	}

	public void stop() {
		if (m_google_api_client.isConnected()) {
      		m_google_api_client.disconnect();
   		}
	}

	public String get_player() {
		try {
			Player p = Games.Players.getCurrentPlayer(m_google_api_client);
			if (p!=null) {
				JSONObject obj = new JSONObject();
	            obj.put("name",p.getDisplayName());
	            obj.put("id",p.getPlayerId());
	            obj.put("avatar",p.getIconImageUri().toString());
	            return obj.toString();
			}
			
		} catch (Exception e) {
			Log.e(TAG, "get_player() failed! " + e.toString());
		}
		return null;
	}

	public boolean send_scores(String data) {
		try {
			if (m_google_api_client.isConnected()) {
				JSONObject jsonObject = new JSONObject(data);
				for(Iterator<String> iter = jsonObject.keys();iter.hasNext();) {
					String key = iter.next();
					long val = jsonObject.getLong(key);
					Log.i(TAG,"send_scores : " + key + "->" + val);
					Games.Leaderboards.submitScore(m_google_api_client, key, val);
				}
				return true;
			}
		} catch (Exception e) {
			Log.e(TAG, "send_scores() failed! " + e.toString());
		}
		return false;
	}

	public boolean send_achievements(String data) {
		try {
			if (m_google_api_client.isConnected()) {
				JSONObject jsonObject = new JSONObject(data);
				for(Iterator<String> iter = jsonObject.keys();iter.hasNext();) {
					String key = iter.next();
					long val = jsonObject.getLong(key);
					if (val >= 100) {
						Log.i(TAG,"send_achievements : " + key);
						Games.Achievements.unlock(m_google_api_client, key);
					}
				}
				return true;
			}
		} catch (Exception e) {
			Log.e(TAG, "send_scores() failed! " + e.toString());
		}
		return false;
	}

	public boolean get_friends() {
		try {
			if (m_google_api_client.isConnected()) {
				Games.Players.loadConnectedPlayers(m_google_api_client,false).setResultCallback(this);
			}
		} catch (Exception e) {
			Log.e(TAG, "get_friends() failed! " + e.toString());
		}
		return false;
	}

	public void show_ui(String ui) {
		if (ui.equals("leaderboards")) {
			m_activity.startActivityForResult(Games.Leaderboards.getAllLeaderboardsIntent(m_google_api_client), REQUEST_LEADERBOARD);
		} else if (ui.equals("achievements")) {
			m_activity.startActivityForResult(Games.Achievements.getAchievementsIntent(m_google_api_client), REQUEST_ACHIEVEMENTS);
		}
	}

	@Override
	public void onConnected(Bundle bundle) {
		Log.d(TAG, "onConnected() called. Sign in successful!");
		nativeSignInResponse("success");
		Player p = Games.Players.getCurrentPlayer(m_google_api_client);
	}
	@Override
	public void onConnectionSuspended(int i) {
		Log.d(TAG, "onConnectionSuspended() called. Trying to reconnect.");
		m_google_api_client.connect();
	}
	@Override
	public void onConnectionFailed(ConnectionResult connectionResult) {
		Log.d(TAG, "onConnectionFailed() called, result: " + connectionResult);

		if (m_resolving_connection_failure) {
			Log.d(TAG, "onConnectionFailed() ignoring connection failure; already resolving.");
			return;
		}

		if (m_signin_called || m_first_login_flow) {
			m_signin_called = false;
			m_first_login_flow = false;
			m_resolving_connection_failure = resolveConnectionFailure(
				connectionResult, RC_SIGN_IN);
		} else {
			nativeSignInResponse("failed");
		}
	}

	@Override 
    public void onResult(Players.LoadPlayersResult res) {
    	if (res.getStatus().isSuccess()) {
    		try {
	    		PlayerBuffer players = res.getPlayers();
	    		JSONArray a = new JSONArray();
	    		int count = players.getCount();
	    		for (int i=0;i<count;i++) {
	    			Player p = players.get(i);
	    			JSONObject obj = new JSONObject();
	    			obj.put("name",p.getDisplayName());
	            	obj.put("id",p.getPlayerId());
	            	obj.put("avatar",p.getIconImageUri().toString());
	    			a.put(obj);
	    		}
	    		nativeGetFriendsResponse(a.toString());
	    	} catch (Exception e) {
	    		Log.e(TAG, "onResult() failed. " + e.toString());
	    	}
    	} else {
    		nativeGetFriendsResponse("failed");
    	}
    }

	public boolean handleActivityResult(int requestCode, int responseCode, Intent intent) {
	    if (requestCode == RC_SIGN_IN) {
	      Log.d(TAG, "onActivityResult with requestCode == RC_SIGN_IN, responseCode="
	          + responseCode + ", intent=" + intent);
	      m_signin_called = false;
	      m_resolving_connection_failure = false;
	      if (responseCode == Activity.RESULT_OK) {
	        	m_google_api_client.connect();
	      } else {
	      		nativeSignInResponse("failed");
	      }
	      return true;
	    } else if (requestCode == REQUEST_LEADERBOARD) {
	    	return true;
	    } else if (requestCode == REQUEST_ACHIEVEMENTS) {
	    	return true;
	    }
	    return false;
	}


	boolean resolveConnectionFailure(ConnectionResult result, int requestCode) {

        if (result.hasResolution()) {
            try {
                result.startResolutionForResult(m_activity, requestCode);
                return true;
            } catch (IntentSender.SendIntentException e) {
                // The intent was canceled before it was sent.  Return to the default
                // state and attempt to connect to get an updated ConnectionResult.
                m_google_api_client.connect();
                return false;
            }
        } else {
            // not resolvable... so show an error message
            int errorCode = result.getErrorCode();
            Dialog dialog = GooglePlayServicesUtil.getErrorDialog(errorCode,
                    m_activity, requestCode);
            if (dialog != null) {
                dialog.show();
            } 
            return false;
        }
    }



	private static native void nativeSignInResponse(String status);
	private static native void nativeGetFriendsResponse(String status);

}