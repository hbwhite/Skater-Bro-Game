LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)
$(call import-add-path, $(LOCAL_PATH))

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := $(LOCAL_PATH)/hellocpp/main.cpp \
$(LOCAL_PATH)/../../../Classes/AppDelegate.cpp \
$(LOCAL_PATH)/../../../Classes/GameScene.cpp \
$(LOCAL_PATH)/../../../Classes/TexturePacker/Images.cpp \
$(LOCAL_PATH)/../../../Classes/PhysicsLoader/PhysicsLoader.cpp \
$(LOCAL_PATH)/../../../Classes/HWAlert/HWAlert-Android.cpp \
$(LOCAL_PATH)/../../../Classes/HWAvailability/HWAvailability-Android.cpp \
$(LOCAL_PATH)/../../../Classes/HWURLOpen/HWURLOpen-Android.cpp

LOCAL_CPPFLAGS := -DSDKBOX_ENABLED
LOCAL_LDLIBS := -landroid \
-llog
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes \
$(LOCAL_PATH)/../../../Classes/TexturePacker \
$(LOCAL_PATH)/../../../Classes/PhysicsLoader \
$(LOCAL_PATH)/../../../Classes/HWAlert \
$(LOCAL_PATH)/../../../Classes/HWAvailability \
$(LOCAL_PATH)/../../../Classes/HWURLOpen

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_WHOLE_STATIC_LIBRARIES += sdkbox PluginSdkboxAds PluginIAP sdkbox PluginReview sdkbox
LOCAL_WHOLE_STATIC_LIBRARIES += PluginAdMob
LOCAL_WHOLE_STATIC_LIBRARIES += PluginSdkboxPlay
LOCAL_WHOLE_STATIC_LIBRARIES += PluginShare
LOCAL_WHOLE_STATIC_LIBRARIES += PluginFacebook

LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module, ./sdkbox)
$(call import-module, ./pluginsdkboxplay)
$(call import-module, ./pluginadmob)
$(call import-module, ./pluginshare)
$(call import-module, ./pluginfacebook)
$(call import-module, ./pluginsdkboxads)
$(call import-module, ./pluginiap)
$(call import-module, ./pluginreview)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
