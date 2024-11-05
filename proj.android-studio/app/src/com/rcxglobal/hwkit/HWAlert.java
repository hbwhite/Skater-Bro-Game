package com.rcxglobal.hwkit;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.DialogInterface;
import android.os.Build;
import android.util.Log;

import java.util.Arrays;

import static org.cocos2dx.lib.Cocos2dxActivity.getContext;

/**
 * Created by harrison on 4/9/18.
 */

public class HWAlert {
    public native static void alertClickedButtonAtIndex(long alert, int index);

    /**
     * Get activity instance from desired context.
     */
    public static Activity getActivity(Context context) {
        if (context == null) return null;
        if (context instanceof Activity) return (Activity) context;
        if (context instanceof ContextWrapper) return getActivity(((ContextWrapper)context).getBaseContext());
        return null;
    }

    public static void showAlert(final long alert, String title, String message, String cancelButtonTitle, String[] otherButtonTitles) {

        Log.i("HWAlert", String.format("showAlert title %s message %s cancel %s buttons %s", title, message, cancelButtonTitle, Arrays.toString(otherButtonTitles)));

        Context context = getContext();

        // https://stackoverflow.com/questions/44426676/why-alertdialog-displays-blank-message

        AlertDialog.Builder builder;
        if (Build.VERSION.SDK_INT >= 21) {
            builder = new AlertDialog.Builder(context, android.R.style.Theme_Material_Light_Dialog_Alert);
        } else {
            builder = new AlertDialog.Builder(context);
        }

        // index 0 always represents the cancel button

        builder.setTitle(title)
               .setMessage(message);

        // merely calling setItems() will cause the alert to put a list view in place of the
        // message, so only call it if there are other buttons to show

        if (otherButtonTitles.length > 2) {
            // this doesn't seem to work
            builder.setItems(otherButtonTitles, (dialogInterface, i) -> {
                // index 0 is reserved for the cancel button on iOS
                alertClickedButtonAtIndex(alert, i);
            });
            builder.setNegativeButton(cancelButtonTitle, (dialog, which) -> alertClickedButtonAtIndex(alert, 0));
        } else {
            if (otherButtonTitles.length < 2) {
                // if the alert only has 1 button, making the cancel button the negative button puts
                // it on the right side where it feels more natural to tap

                if (otherButtonTitles.length == 1) {
                    builder.setPositiveButton(otherButtonTitles[0], (dialogInterface, i) -> {
                        alertClickedButtonAtIndex(alert, 1);
                    });
                }
                builder.setNegativeButton(cancelButtonTitle, (dialog, which) -> alertClickedButtonAtIndex(alert, 0));
            } else {
                // this odd (neutral = cancel, positive = button 1, negative = button 2) setup puts the
                // cancel button at the bottom and buttons 1 and 2 above it

                builder.setPositiveButton(otherButtonTitles[0], (dialogInterface, i) -> {
                    alertClickedButtonAtIndex(alert, 1);
                });
                builder.setNegativeButton(otherButtonTitles[1], (dialogInterface, i) -> {
                    alertClickedButtonAtIndex(alert, 2);
                });
                builder.setNeutralButton(cancelButtonTitle, (dialog, which) -> alertClickedButtonAtIndex(alert, 0));
            }
        }

        // must show alert on the main thread

        Activity activity = getActivity(context);
        if (activity != null) {
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    builder.show();
                }
            });
        }
    }
}
