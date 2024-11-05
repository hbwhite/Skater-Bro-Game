//
//  HWURLOpen-Android.cpp
//  skaterbro-mobile
//
//  Created by Harrison White on 4/5/18.
//

#include <platform/CCStdC.h>
#include <platform/android/jni/JniHelper.h>
#include <jni.h>
#include "HWURLOpen.h"

void HWURLOpen::openURL(const char *url) {

    cocos2d::JniMethodInfo methodInfo;
    bool success = cocos2d::JniHelper::getStaticMethodInfo(methodInfo, "com/rcxglobal/hwkit/HWURLOpen", "openURL", "(Ljava/lang/String;)V");

    if (!success) {
        CCLOG("Could not load method information. Wrong signature?");
        return;
    }
    
    jstring javaURL = methodInfo.env->NewStringUTF(url);

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, javaURL);

    methodInfo.env->DeleteLocalRef(methodInfo.classID);
    methodInfo.env->DeleteLocalRef(javaURL);
}
