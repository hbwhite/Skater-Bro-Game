package com.rcxglobal.hwkit;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.ContextWrapper;
import android.util.Log;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;

import static org.cocos2dx.lib.Cocos2dxActivity.getContext;

/**
 * Created by harrison on 4/9/18.
 */

public class HWAvailability {
    // arbitrary "activity request code" (handle) passed back in onActivityResult callback
    private static final int PLAY_SERVICES_RESOLUTION_REQUEST = 4644;

    // https://stackoverflow.com/questions/22493465/check-if-correct-google-play-service-available-unfortunately-application-has-s

    /**
     * Get activity instance from desired context.
     */
    public static Activity getActivity(Context context) {
        if (context == null) return null;
        if (context instanceof Activity) return (Activity) context;
        if (context instanceof ContextWrapper) return getActivity(((ContextWrapper)context).getBaseContext());
        return null;
    }

    public static boolean isGooglePlayAvailable() {
        Context context = getContext();
        Activity activity = getActivity(context);

        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();
        int resultCode = apiAvailability.isGooglePlayServicesAvailable(context);
        if (resultCode != ConnectionResult.SUCCESS) {
            if (apiAvailability.isUserResolvableError(resultCode)) {
                apiAvailability.getErrorDialog(activity, resultCode, PLAY_SERVICES_RESOLUTION_REQUEST)
                        .show();
            } else {
                HWAlert.showAlert(0,
                        "Device Not Supported",
                        "This device does not support Google Play, which is required to view the leaderboard.",
                        "OK",
                        null);
            }
            return false;
        }
        return true;
    }
}
