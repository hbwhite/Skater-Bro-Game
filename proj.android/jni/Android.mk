LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos/audio/include)
$(call import-add-path, $(LOCAL_PATH))

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := hellocpp/main.cpp \
../../Classes/AppDelegate.cpp \
../../Classes/HelloWorldScene.cpp \
../../Classes/PluginSdkboxAdsJS.cpp \
../../Classes/PluginSdkboxAdsJSHelper.cpp \
../../Classes/SDKBoxJSHelper.cpp \
../../Classes/PluginFacebookJS.cpp \
../../Classes/PluginFacebookJSHelper.cpp \
../../Classes/PluginSdkboxPlayJS.cpp \
../../Classes/PluginSdkboxPlayJSHelper.cpp \
../../Classes/PluginShareJS.cpp \
../../Classes/PluginShareJSHelper.cpp \
../../Classes/PluginAdMobJS.cpp \
../../Classes/PluginAdMobJSHelper.cpp \
../../Classes/PluginIAPJS.cpp \
../../Classes/PluginIAPJSHelper.cpp \
../../Classes/PluginReviewJS.cpp \
../../Classes/PluginReviewJSHelper.cpp

LOCAL_CPPFLAGS := -DSDKBOX_ENABLED
LOCAL_LDLIBS := -landroid \
-llog
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_WHOLE_STATIC_LIBRARIES += PluginSdkboxAds PluginShare PluginFacebook PluginSdkboxAds PluginFacebook PluginSdkboxAds PluginShare PluginSdkboxAds PluginIAP PluginReview
LOCAL_WHOLE_STATIC_LIBRARIES += sdkbox
LOCAL_WHOLE_STATIC_LIBRARIES += PluginSdkboxPlay
LOCAL_WHOLE_STATIC_LIBRARIES += PluginAdMob

LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module, ./sdkbox)
$(call import-module, ./pluginsdkboxads)
$(call import-module, ./pluginsdkboxplay)
$(call import-module, ./pluginadmob)
$(call import-module, ./pluginshare)
$(call import-module, ./pluginfacebook)
$(call import-module, ./pluginiap)
$(call import-module, ./pluginreview)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
