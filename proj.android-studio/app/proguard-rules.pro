# Add project specific ProGuard rules here.
# By default, the flags in this file are appended to flags specified
# in E:\developSoftware\Android\SDK/tools/proguard/proguard-android.txt
# You can edit the include path and order by changing the proguardFiles
# directive in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# Add any project specific keep options here:

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}
#sdkbox
-keep class com.sdkbox.** { *; }
-dontwarn com.sdkbox.**
# share
-keep class com.share.** { *; }
-dontwarn com.share.**
#twitter
-keep public com.twitter.sdk.android.** { *; }
#sdkbox
-keep class com.sdkbox.** { *; }
-dontwarn com.sdkbox.**
# google play service
-keep class com.google.android.gms.** { *; }
-dontwarn com.google.android.gms.**
-keep class com.google.protobuf.** { *; }
-dontwarn com.google.protobuf.**
-keep class * extends java.util.ListResourceBundle {
    protected Object[][] getContents();
}
-keep public class com.google.android.gms.common.internal.safeparcel.SafeParcelable {
    public static final *** NULL;
}
-keepnames @com.google.android.gms.common.annotation.KeepName class *
-keepclassmembernames class * {
    @com.google.android.gms.common.annotation.KeepName *;
}
-keepnames class * implements android.os.Parcelable {
    public static final ** CREATOR;
}
#sdkbox
-keep class com.sdkbox.** { *; }
-dontwarn com.sdkbox.**
# facebook
-keep public class com.facebook.** { *; }
-dontwarn com.facebook.**
#sdkbox
-keep class com.sdkbox.** { *; }
-dontwarn com.sdkbox.**
# unityads
# Keep filenames and line numbers for stack traces
-keepattributes SourceFile,LineNumberTable
# Keep JavascriptInterface for WebView bridge
-keepattributes JavascriptInterface
# Sometimes keepattributes is not enough to keep annotations
-keep class android.webkit.JavascriptInterface {
   *;
}
# Keep all classes in Unity Ads package
-keep class com.unity3d.ads.** {
   *;
}
# google play service
-keep class com.google.android.gms.** { *; }
-dontwarn com.google.android.gms.**
-keep class com.google.protobuf.** { *; }
-dontwarn com.google.protobuf.**
-keep class * extends java.util.ListResourceBundle {
    protected Object[][] getContents();
}
-keep public class com.google.android.gms.common.internal.safeparcel.SafeParcelable {
    public static final *** NULL;
}
-keepnames @com.google.android.gms.common.annotation.KeepName class *
-keepclassmembernames class * {
    @com.google.android.gms.common.annotation.KeepName *;
}
-keepnames class * implements android.os.Parcelable {
    public static final ** CREATOR;
}
#sdkbox
-keep class com.sdkbox.** { *; }
-dontwarn com.sdkbox.**

# Crashlytics
#-keepattributes *Annotation*
#-keepattributes SourceFile,LineNumberTable
#-keep public class * extends java.lang.Exception
#-keepresourcexmlelements manifest/application/meta-data@name=io.fabric.ApiKey
#-printmapping mapping.txt
#-keep class com.crashlytics.** { *; }
#-dontwarn com.crashlytics.**
