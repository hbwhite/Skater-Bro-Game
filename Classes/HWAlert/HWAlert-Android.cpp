//
//  HWAlert-Android.cpp
//  skaterbro
//
//  Created by Harrison White on 4/6/18.
//

#include "HWAlert-Android.h"

// https://medium.com/@teratapgames/integrating-java-and-c-with-jni-with-cocos2d-x-a-no-bullshit-straightforward-tutorial-7171e2c10455

#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <cstdlib>      /* NULL */
#include <stdarg.h>     /* va_arg */
#include "cocos2d.h"    /* CCLOG */

// http://discuss.cocos2d-x.org/t/call-c-function-from-java/27255

//package com.mypackage;
//public class MyClass {
//public native void myCppFunction();
//}

#ifdef __cplusplus
extern "C" {
#endif
void Java_com_rcxglobal_hwkit_HWAlert_alertClickedButtonAtIndex(JNIEnv *env, jobject thiz, jlong alert, jint index) {
    // Java native callback

    // need to reference the instance of HWAlert that triggered this callback
    // use a static global var in this file,
    // init that var by overriding the constructor here and calling its original code in the .h file

    if (alert == 0) {
        // in case HWAlert was called directly in Java, return here and do not attempt callback to C++
        return;
    }

    // notify the listener of the Alert C++ instance that triggered this alert dialog
    HWAlertAndroid* hwAlertAndroid = (HWAlertAndroid*)alert;
    HWAlertCallback callback = hwAlertAndroid->getCallback();
    if (callback != nullptr) {
        callback(hwAlertAndroid, index);
    }

    CCLOG("[HWAlert-Android.cpp] alert %p clicked %d", hwAlertAndroid, index);

    delete hwAlertAndroid; // automatically manage memory
}

#ifdef __cplusplus
}
#endif

// HWAlert Interface

HWAlert* HWAlert::create(const char* title, const char* message, const char* cancelButtonTitle, ...) {
    va_list args;
    va_start(args, cancelButtonTitle);
    HWAlert* retval = HWAlertAndroid::create(title, message, cancelButtonTitle, args);
    va_end(args);
    return retval;
}

// public static method used to access private members of HWAlertAndroid through HWAlert::create

HWAlertAndroid* HWAlertAndroid::create(const char* title, const char* message, const char* cancelButtonTitle, va_list otherButtonTitles) {
    // automatically manage memory
    HWAlertAndroid* hwAlert = new HWAlertAndroid(title, message, cancelButtonTitle);

    // handle variadic args
    for (const char* button = va_arg(otherButtonTitles, const char*); button != NULL; button = va_arg(otherButtonTitles, const char*)) {
        hwAlert->otherButtonTitles.push_back(button);
    }

    return hwAlert;
}

HWAlertAndroid::HWAlertAndroid(const char* title, const char* message, const char* cancelButtonTitle)
    :
    title(title),
    message(message),
    cancelButtonTitle(cancelButtonTitle),
    otherButtonTitles(std::vector<const char*>())
{
    
}

void HWAlertAndroid::show() {

    // showAlert() takes args:
    // String title, String message, String cancelButtonTitle, String[] otherButtonTitles

    cocos2d::JniMethodInfo methodInfo;
    bool success = cocos2d::JniHelper::getStaticMethodInfo(methodInfo, "com/rcxglobal/hwkit/HWAlert", "showAlert", "(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)V");

    if (!success) {
        CCLOG("Could not load method information. Wrong signature?");
        return;
    }

    jlong javaAlert = (jlong)this;
    jstring javaTitle = methodInfo.env->NewStringUTF(title);
    jstring javaMessage = methodInfo.env->NewStringUTF(message);
    jstring javaCancelButtonTitle = methodInfo.env->NewStringUTF(cancelButtonTitle);

    auto stringClass = methodInfo.env->FindClass("java/lang/String");
    jobjectArray javaOtherButtonTitles = methodInfo.env->NewObjectArray((jsize)otherButtonTitles.size(), stringClass, 0);

    for (int i = 0; i < (jsize)otherButtonTitles.size(); i++) {
        jstring javaButtonTitle = methodInfo.env->NewStringUTF(otherButtonTitles.at(i));
        methodInfo.env->SetObjectArrayElement(javaOtherButtonTitles, i, javaButtonTitle);
    }

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, javaAlert, javaTitle, javaMessage, javaCancelButtonTitle, javaOtherButtonTitles);

    methodInfo.env->DeleteLocalRef(methodInfo.classID);
    methodInfo.env->DeleteLocalRef(javaTitle);
    methodInfo.env->DeleteLocalRef(javaMessage);
    methodInfo.env->DeleteLocalRef(javaCancelButtonTitle);
    methodInfo.env->DeleteLocalRef(javaOtherButtonTitles);
}
