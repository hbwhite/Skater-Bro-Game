/****************************************************************************
Copyright (c) 2015-2017 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.multidex.MultiDex;
import android.util.Log;

import org.cocos2dx.lib.BuildConfig;
import org.cocos2dx.lib.Cocos2dxActivity;

// Crashlytics
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.ConsumeResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.SkuDetails;
import com.android.billingclient.api.SkuDetailsParams;
import com.android.billingclient.api.SkuDetailsResponseListener;
import com.crashlytics.android.Crashlytics;
import com.crashlytics.android.ndk.CrashlyticsNdk;
import com.google.android.gms.ads.AdActivity;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.MobileAds;
import com.mopub.common.MoPub;
import com.mopub.common.SdkConfiguration;

import java.util.Arrays;
import java.util.List;

import io.fabric.sdk.android.Fabric;

public class AppActivity extends com.sdkbox.plugin.SDKBoxActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.setEnableVirtualButton(false);
        super.onCreate(savedInstanceState);
        //
        // When using a Splash Screen activity that launches AppActivity, this may happen:
        //
        // FATAL EXCEPTION: main
        // java.lang.RuntimeException: Unable to destroy activity {io.mindlessgames.skaterbro/org.cocos2dx.cpp.AppActivity}: java.lang.NullPointerException
        // at android.app.ActivityThread.performDestroyActivity(ActivityThread.java:3632)
        //
        // I found this issue via pre-launch report on Google Play. The fix is to comment out
        // the isTaskRoot() block below. Discussed here:
        // https://github.com/cocos2d/cocos2d-x/issues/18884
        //
        // However, in the end I got rid of the SplashActivity and just changed the theme of the
        // AppActivity to have a splash screen background. My reasoning is as follows:
        //
        // On simple apps, the main activity probably loads instantly, which requires them to
        // use a separate activity for the splash screen. However, my game's AppActivity takes
        // a moment to load. For that reason, 1-2 seconds would be spent looking at the black
        // background of the AppActivity after the splash screen disappeared, which didn't make
        // much sense because the game should be loading during the splash screen.
        // Another issue I had was that the splash screen would load every time the app was opened,
        // even if the game was in the background, and I didn't find a good solution (that worked)
        // for checking if the game was already open to skip the delay on the splash screen.
        //
        // So now the app works with just AppActivity and it has a custom background set to
        // the splash screen, which works great. There is no delay on a black screen while
        // AppActivity loads, and the user only sees the splash screen when the app first launches,
        // not every time the app is opened (in case it's already running in the background).
        //

        // Workaround in https://stackoverflow.com/questions/16283079/re-launch-of-activity-on-home-button-but-only-the-first-time/16447508
        if (!isTaskRoot()) {
            // Android launched another instance of the root activity into an existing task
            //  so just quietly finish and go away, dropping the user back into the activity
            //  at the top of the stack (ie: the last state of this task)
            // Don't need to finish it again since it's finished in super.onCreate .
            return;
        }
        // DO OTHER INITIALIZATION BELOW

        // MultiDex support for Fabric.io plugins
        // https://stackoverflow.com/questions/32097150/fabric-crashlytics-noclassdeffounderror-only-on-certain-devices
        // https://twittercommunity.com/t/noclassdeffounderror-when-add-twitter-fabric-sdk/51834/3
        MultiDex.install(this);

        // Crashlytics
        // Fabric.with(this, new Crashlytics());
        Fabric.with(this, new Crashlytics(), new CrashlyticsNdk());

        // testing
        // Fabric.getKit(Crashlytics.class).crash();

        // MoPub
        // I think you only need one of the ad unit IDs here
        // other add units will be served from the IDs in AdMob Mediation
        SdkConfiguration sdkConfiguration =
                new SdkConfiguration.Builder("f16ebaf82197436289036410e01f6ec2").build();

        // getContext() can be null which will cause initializeSdk() to crash,
        // so we're passing in the activity itself instead
        MoPub.initializeSdk(this, sdkConfiguration, null);

        // TESTING ONLY
        if (BuildConfig.DEBUG) { // safeguard
            clearInAppPurchases();
        }
    }

    private void clearInAppPurchases() {
        // TESTING ONLY
        // Used to "consume" non-consumable in-app purchases (such as "remove ads")
        // in order to be able to purchase them again on this account during testing.

        final String TAG = "clearInAppPurchases";

        BillingClient mBillingClient = BillingClient.newBuilder(this).setListener(new PurchasesUpdatedListener() {
            @Override
            public void onPurchasesUpdated(int responseCode, @Nullable List<Purchase> purchases) {
                int purchasesLength = 0;
                if (purchases != null) {
                    purchasesLength = purchases.size();
                }
                Log.d(TAG, String.format("onPurchasesUpdated code %d purchases length %d", responseCode, purchasesLength));
            }
        }).build();
        mBillingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingSetupFinished(int billingSetupResponseCode) {
                if (billingSetupResponseCode == BillingClient.BillingResponse.OK) {
                    Log.d(TAG, "onBillingSetupFinished() response code " + billingSetupResponseCode);

                    Purchase.PurchasesResult purchasesResult = mBillingClient.queryPurchases(BillingClient.SkuType.INAPP);
                    List<Purchase> purchases = purchasesResult.getPurchasesList();
                    if (purchases != null) {
                        for (Purchase sourcePurchase : purchases) {
                            if (sourcePurchase != null) {
                                Log.i(TAG, "consuming purchase " + sourcePurchase.getSku());
                                ConsumeResponseListener listener = (consumeResponseCode, purchaseToken) -> {
                                    Log.d(TAG, String.format("onConsumeResponse code %d token %s", consumeResponseCode, purchaseToken));

                                    if (consumeResponseCode == BillingClient.BillingResponse.OK) {
                                        Log.i(TAG, "successfully consumed purchase " + sourcePurchase.getSku());
                                    } else {
                                        Log.w(TAG, "did not consume purchase " + sourcePurchase);
                                    }
                                };
                                mBillingClient.consumeAsync(sourcePurchase.getPurchaseToken(), listener);
                            } else {
                                Log.w(TAG, "found null when trying to consume purchase");
                            }
                        }
                    } else {
                        Log.w(TAG, "getPurchasesList() returned null");
                    }

                } else {
                    Log.w(TAG, "onBillingSetupFinished() error code: " + billingSetupResponseCode);
                }
            }

            @Override
            public void onBillingServiceDisconnected() {
                Log.w(TAG, "onBillingServiceDisconnected()");
            }
        });
    }

    /*
    public static boolean isGooglePlayAvailable() {
        boolean googlePlayStoreInstalled;
        int val = GooglePlayServicesUtil.isGooglePlayServicesAvailable(LocationActivity.this);
        googlePlayStoreInstalled = val == ConnectionResult.SUCCESS;
        return googlePlayStoreInstalled;
    }
    */
}
