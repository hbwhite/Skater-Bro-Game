#include "AppDelegate.h"
#include "GameScene.h"
#include "Images.hpp"

#ifdef SDKBOX_ENABLED
#include "pluginsdkboxplay/PluginSdkboxPlay.h"
#include "pluginadmob/PluginAdMob.h"
#include "pluginshare/PluginShare.h"
#include "pluginiap/PluginIAP.h"
#include "pluginreview/PluginReview.h"
#endif

// #define USE_AUDIO_ENGINE 1
#define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

// using a standard 16:9 aspect ratio
// as of 2015, 360x640 accounted for the largest proportion of smartphones
static cocos2d::Size designResolutionSize = cocos2d::Size(360, 640);

// use this for simulating larger screens:
// static cocos2d::Size designResolutionSize = cocos2d::Size(640, 1136);

// making the larger resolutions multiples of the smaller resolutions
// makes it easier to scale the artwork without doing calculations

// Android Studio uses mdpi, hdpi, xhdpi, and xxhdpi
// so using 4 different resolutions here seems reasonable
// Use Adobe Illustrator plugin to export at 1, 2, 3, 4x

static cocos2d::Size res1x = cocos2d::Size(360, 640);
static cocos2d::Size res2x = cocos2d::Size(720, 1280);
static cocos2d::Size res3x = cocos2d::Size(1080, 1920);
static cocos2d::Size res4x = cocos2d::Size(1440, 2560);

static const int res1xMaxTextureSize    = 2048;
static const int res2xMaxTextureSize    = 2048;
static const int res3xMaxTextureSize    = 2048;
static const int res4xMaxTextureSize    = 4096;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
#ifdef SDKBOX_ENABLED
    sdkbox::PluginSdkboxPlay::init();
    sdkbox::PluginShare::init();
    sdkbox::PluginFacebook::init();
    sdkbox::IAP::init();
    sdkbox::PluginReview::init();
#ifndef NOADS
    // we need to load admob regardless of whether the user purchased the no-ads item
    // because we still have the option to revive your character with a video ad
    sdkbox::PluginAdMob::init();
#endif
#endif
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#if SCREENSHOT_MODE
        // for screenshots on other device sizes
        // I'm using a retina display so the pixel density is 4x normal resolution;
        // therefore, divide the width and height by 2 for screenshots and screen recordings
        glview = GLViewImpl::createWithRect("Skater Bro", cocos2d::Rect(0, 0, 1536/2, 2048/2));
        //glview->setFrameZoomFactor(0.5);
#else
        glview = GLViewImpl::createWithRect("Skater Bro", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#endif
#else
        glview = GLViewImpl::create("Skater Bro");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);
    
    // based on https://www.codeandweb.com/texturepacker/tutorials/animations-and-spritesheets-in-cocos2d-x
    
    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER /* ResolutionPolicy::EXACT_FIT */ );
    
    // load the smallest resolution that is greater than or equal to
    // the current screen size to prevent upscaling
    
    std::vector<std::string> searchPaths;
    float scaleFactor = 1.0f;
    auto frameSize = glview->getFrameSize();
    
    auto config = Configuration::getInstance();
    int maxTexutureSize = config->getMaxTextureSize();
    CCLOG("[detected max texture size = %d]", maxTexutureSize);
    
    // on retina macs, the window size will be different from the resolution
    // so the window may be 360 x 640 while the resolution is actually 720 x 1280 on the retina display
    // glview->isRetinaDisplay() and glview->getContentScaleFactor() don't seem to work on my computer
    // so I just hardcoded 2x resolution here for desktop computers
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    frameSize.width *= 2;
    frameSize.height *= 2;
#endif
    
    // downgrade the resolution if it cannot support the size of the texture atlas
    // currently the 1x resolution uses a max texture size of 2048x2048,
    // while all the higher resolutions use a max texture size of 4096x4096
    
    if (frameSize.height <= res1x.height || maxTexutureSize < res2xMaxTextureSize) {
        searchPaths.push_back("res/1x");
        scaleFactor = res1x.height/designResolutionSize.height;
        CCLOG("[using 1x resolution] setting scale factor %.2f", scaleFactor);
    } else if (frameSize.height <= res2x.height || maxTexutureSize < res3xMaxTextureSize) {
        searchPaths.push_back("res/2x");
        scaleFactor = res2x.height/designResolutionSize.height;
        CCLOG("[using 2x resolution] setting scale factor %.2f", scaleFactor);
    } else if (frameSize.height <= res3x.height || maxTexutureSize < res4xMaxTextureSize) {
        searchPaths.push_back("res/3x");
        scaleFactor = res3x.height/designResolutionSize.height;
        CCLOG("[using 3x resolution] setting scale factor %.2f", scaleFactor);
    } else {
        searchPaths.push_back("res/4x");
        scaleFactor = res4x.height/designResolutionSize.height;
        CCLOG("[using 4x resolution] setting scale factor %.2f", scaleFactor);
    }
    
    FileUtils::getInstance()->setSearchPaths(searchPaths);
    director->setContentScaleFactor(scaleFactor);
    
    // load sprites after setting the search paths above
    // to select the current device's resolution
    loadSprites();
    
    register_all_packages();
    
    // removed splash screen
    
    // create a scene. it's an autorelease object
    auto gameScene = Game::createScene();
    
    // run
    director->runWithScene(gameScene);
    
    // dope theme music
    // SimpleAudioEngine::getInstance()->playBackgroundMusic("vlc.mp3", true);
    
    return true;
}

void AppDelegate::loadSprites() {
    // TexturePacker
    ZipUtils::setPvrEncryptionKey(0x5bb08169, 0x2ea5c200, 0x05644b0d, 0x89ad2e86); // content protection (ITAR-exempt encryption)
    Image::setPVRImagesHavePremultipliedAlpha(true); // enable premultiply alpha
    TexturePacker::Images::addSpriteFramesToCache(); // load images into SpriteFrame cache
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    // pause game and show pause screen
    Game* game = static_cast<Game*>(Director::getInstance()->getRunningScene());
    game->pauseGame();
    
    Director::getInstance()->stopAnimation();

    // This can end up pausing the user's music
    /*
#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
    */
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // This can end up pausing the user's music
    /*
#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
     */
}
