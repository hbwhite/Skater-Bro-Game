package com.rcxglobal.hwkit;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;

import static org.cocos2dx.lib.Cocos2dxActivity.getContext;

/**
 * Created by harrison on 4/9/18.
 */

public class HWURLOpen {
    public static void openURL(String url) {
        // https://stackoverflow.com/questions/2201917/how-can-i-open-a-url-in-androids-web-browser-from-my-application

        Log.i("HWURLOpen", String.format("opening URL %s", url));

        Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
        getContext().startActivity(browserIntent);
    }
}
