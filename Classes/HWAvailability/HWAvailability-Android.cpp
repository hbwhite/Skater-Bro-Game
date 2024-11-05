//
//  HWAvailability-Android.cpp
//  skaterbro-mobile
//
//  Created by Harrison White on 4/5/18.
//

#include <platform/CCStdC.h>
#include <platform/android/jni/JniHelper.h>
#include "HWAvailability.h"

bool HWAvailability::isLeaderboardAvailable() {

    cocos2d::JniMethodInfo methodInfo;
    bool success = cocos2d::JniHelper::getStaticMethodInfo(methodInfo, "com/rcxglobal/hwkit/HWAvailability", "isGooglePlayAvailable", "()Z");

    if (!success) {
        CCLOG("Could not load method information. Wrong signature?");

        // if we can't determine whether google play is available, assume that it is so the user
        // doesn't get locked out
        return true;
    }

    bool available = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);

    methodInfo.env->DeleteLocalRef(methodInfo.classID);

    return available;
}
