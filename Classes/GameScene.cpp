/**
 * Cameras
 * These are rendered in order, with the last rendered camera's objects on top
 * This is why the background must be in the lowest-level camera, and the UI must be on top
 *
 * USER1 [static camera, center anchored]
 * - Background
 *
 * USER2 - tracking camera [dynamic camera, center anchored]
 * - Player
 * - Obstacles
 *
 * DEFAULT - UI camera [dynamic camera, center anchored]
 * - Score
 * - Pause Button
 *
 
 ******* USERx is intended to be used for game, DEFAULT is for UI as it is rendered last (on top of everything) ******
 
 So:
 fixed elements & ui DEFAULT
 moving elements USER2
 background USER2
 */

#include "GameScene.h"
#include "Background.hpp"
#include "SimpleAudioEngine.h"
#include "Scoreboard.hpp"
#include "PhysicsManager.hpp"
#include "Constants.hpp"
#include "Platform.hpp"
#include "GameButton.hpp"

#include "HWAlert.h"
#include "HWAvailability.h"
#include "HWURLOpen.h"

// c++
#include <iostream>

// for strcpy()
#include <string.h>

// TexturePacker
#include "Images.hpp"
using namespace TexturePacker;

// PhysicsEditor Loader
#include "PhysicsLoader.h"

// Audio Engine (from AppDelegate)
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;

// NO_ADS_IAP_NAME is the alias of the in-app purchase in the sdkbox_config.json file
// *** THIS CANNOT HAVE SPACES OR IT WILL FAIL ON iOS!! (e.g. "No Ads" should be "no_ads")
// e.g. if the file says "iap": { "no_ads": { "id": "io.mindlessgames.skaterbro.no_ads" } }
// it would be "no_ads"

// extern for app delegate
static const char*  NO_ADS_PURCHASED_KEY    = "no_ads_purchased";
static const std::string NO_ADS_IAP_NAME    = "no_ads";
static const std::string NO_ADS_PRODUCT_ID  = "io.mindlessgames.skaterbro.no_ads";
static const int INTERSTITIAL_FREQUENCY     = 3; /* # of games before interstitial */

static const std::string BANNER_AD_NAME     = "banner";
static const std::string GAME_OVER_AD_NAME  = "gameover";
static const std::string REVIVE_AD_NAME     = "revive";

static const char* PRIVACY_POLICY_URL       = "https://s3.amazonaws.com/rcx-llc/skaterbro/privacy_policy.html";

#ifdef SDKBOX_ENABLED 
using namespace sdkbox;
static const char* LEADERBOARD_NAME         = "High Scores";
#endif

// keys for local preferences (saves high scores locally)
static const char*  HIGH_SCORE_KEY          = "highscore";
static const char*  SOUND_KEY               = "sound";
static const bool   SOUND_DEFAULT           = true;

static const std::string ACH_BRONZE         = "Bronze Bro Award";
static const std::string ACH_SILVER         = "Silver Bro Award";
static const std::string ACH_GOLD           = "Golden Bro Award";
static const std::string ACH_PLATINUM       = "Platinum Bro Award";

static const int    PLAYER_W                = 34;
static const int    PLAYER_X_OFFSET         = 67;
static const float  TAP_TIME_FUDGE_FACTOR   = 0.2f;

static const int MENU_PLAYER_X_START        = PLAYER_X_OFFSET*2;
static const int MENU_PLAYER_X_DELTA        = PLAYER_W*10;

// dimensions for design resolution
static const int    TILE_SIZE_W             = 40;
static const int    TILE_SIZE_H             = 40;

static const float  FADE_TIME               = 0.08f;

static const int    LEVEL_JUMP_DISTANCE     = 243; /* distance jumped on the same level -- used to calculate next platform's x */

// sfx
static const char* SFX_JUMP                 = "sfx/jump.mp3";
static const char* SFX_PING                 = "sfx/ping.mp3";
static const char* SFX_GAMEOVER             = "sfx/gameover.mp3";

// We now save games in the local device's user defaults because many devices in China do not have the
// Google Play Services app installed. This way, they can still play the game with high scores.
// Also, it loads data immediately, avoiding a race condition.
// When an Android app is deleted, its Saved Games are deleted as well (this happens on NFSMW).
// Also, iCloud backs up app user defaults, so this solution isn't any worse than Saved Games.

#define SCREENSHOT_MODE 0

#if SCREENSHOT_MODE
#warning Turn off screenshot mode!
#endif

#ifdef SDKBOX_ENABLED

// play adapter
void Game::onConnectionStatusChanged(int status) {
    CCLOG("connection status %d", status);

    if (status == GPS_CONNECTED) {
        if (waitingForLeaderboard) {
            CCLOG("user was waiting for leaderboard; showing");

            waitingForLeaderboard = false;
            PluginSdkboxPlay::showLeaderboard(LEADERBOARD_NAME);
        }
    }
}

// no ads
void Game::grantNoAds(bool restored) {
    HWAlert::create(restored ? "Restore Successful" : "Purchase Successful",
                    "You will no longer see banner ads or fullscreen ads in the app. "
                    "You can still watch video ads to revive your player if you want to.",
                    "OK", NULL)->show();

    UserDefault* defaults = UserDefault::getInstance();
    defaults->setBoolForKey(NO_ADS_PURCHASED_KEY, true);
    defaults->flush();

    PluginAdMob::hide(BANNER_AD_NAME);
    PluginAdMob::hide(GAME_OVER_AD_NAME);
    PluginAdMob::hide(REVIVE_AD_NAME);
}

// iap adapter
void Game::onSuccess(sdkbox::Product const& p) {
    // user purchased product
    CCLOG("iap onSuccess");

    if (p.id == NO_ADS_PRODUCT_ID) {
        grantNoAds(false);
    }
}

void Game::onFailure(sdkbox::Product const& p, const std::string &msg) {
    CCLOG("iap onFailure, msg %s", msg.c_str());

    if (p.id == NO_ADS_PRODUCT_ID && msg == "7 item already owned") {
        // the user has already purchased this item but attempted to purchase it again
        // if you attempt to restore it for them here (immediately), it will fail internally,
        // so show them this alert instead
        HWAlert::create("Item Already Purchased",
                        "You have already purchased this item. To restore it, select No Ads > Restore.",
                        "OK", NULL)->show();
    }
    // if an error occurs, it is typically reported to the user by the system
//    else {
//        HWAlert::create("Error",
//                        "An error occurred. Please check your Internet connection or try again later.",
//                        "OK", NULL)->show();
//    }
}

void Game::onCanceled(sdkbox::Product const& p) {
    CCLOG("iap onCanceled");
}

void Game::onRestored(sdkbox::Product const& p) {
    CCLOG("iap onRestored");

    if (p.id == NO_ADS_PRODUCT_ID) {
        didRestorePurchases = true;
        grantNoAds(true);
    }
}

void Game::onProductRequestSuccess(std::vector<sdkbox::Product> const &products) {
    CCLOG("iap onProductRequestSuccess");

    for (int i = 0; i < products.size(); i++) {
        auto product = products.at(i);
        if (product.id == NO_ADS_PRODUCT_ID && product.price.size() > 0) {
            // If using c_str(),
            // MUST copy product.price.c_str(), as the char array will point to garbage data
            // as soon as product goes out of scope
            // I just used std::string instead so the object would be copied
            noAdsPrice = product.price;
        }
    }
}

void Game::onProductRequestFailure(const std::string &msg) {
    CCLOG("iap onProductRequestFailure");
}

void Game::onRestoreComplete(bool ok, const std::string &msg) {
    CCLOG("iap onRestoreComplete");
    
    if (!didRestorePurchases) {
        if (ok) {
            // probably nothing to restore (didn't buy / wrong account?)
            HWAlert::create("No Purchases to Restore",
                            "There are no purchases to restore from this account. To purchase No Ads, select No Ads > Purchase.",
                            "OK", NULL)->show();
        } else {
            // probably a network issue
            CCLOG("error was %s", msg.c_str());
            HWAlert::create("Error",
                            msg.c_str(),
                            "OK", NULL)->show();
        }
    }
}

// share
void Game::onShareState(const sdkbox::SocialShareResponse& response) {
    switch (response.state) {
        case sdkbox::SocialShareState::SocialShareStateNone: {
            CCLOG("SharePlugin::onShareState none");
            break;
        }
        case sdkbox::SocialShareState::SocialShareStateUnkonw: {
            CCLOG("SharePlugin::onShareState unkonw");
            break;
        }
        case sdkbox::SocialShareState::SocialShareStateBegin: {
            CCLOG("SharePlugin::onShareState begin");
            break;
        }
        case sdkbox::SocialShareState::SocialShareStateSuccess: {
            CCLOG("SharePlugin::onShareState success");
            break;
        }
        case sdkbox::SocialShareState::SocialShareStateFail: {
            CCLOG("SharePlugin::onShareState fail, error:%s", response.error.c_str());
            HWAlert::create("Facebook Share Failed", "Please make sure you have the Facebook app installed.", "OK", NULL)->show();
            break;
        }
        case sdkbox::SocialShareState::SocialShareStateCancelled: {
            CCLOG("SharePlugin::onShareState cancelled");
            break;
        }
        case sdkbox::SocialShareStateSelectShow: {
            CCLOG("SharePlugin::onShareState show panel %d", response.platform);
            break;
        }
        case sdkbox::SocialShareStateSelectCancelled: {
            CCLOG("SharePlugin::onShareState show pancel cancelled %d", response.platform);
            break;
        }
        case sdkbox::SocialShareStateSelected: {
            CCLOG("SharePlugin::onShareState show pancel selected %d", response.platform);
            break;
        }
        default: {
            CCLOG("SharePlugin::onShareState");
            break;
        }
    }
}

// ads

void showText(const std::string &text) {
    CCLOG("%s", text.c_str());
}

void Game::adViewDidReceiveAd(const std::string &name) {
    showText(StringUtils::format("%s name=%s", __FUNCTION__, name.c_str()));

    // set reviveAdLoaded = true regardless of whether the user purchased the no-ads item
    // because they can still revive their character with a video ad if they choose to.
    // gameoverAdLoaded is also set here so the gameover ad will load in case a video ad can't be loaded
    if (name == REVIVE_AD_NAME) {
        reviveAdLoaded = true;
    } else if (name == GAME_OVER_AD_NAME) {
        gameoverAdLoaded = true;
    }

    if (!UserDefault::getInstance()->getBoolForKey(NO_ADS_PURCHASED_KEY, false)) {
        // skip loading ads if the user purchased the no-ads item
        if (name == BANNER_AD_NAME) {
            PluginAdMob::show(name);
        }
    }
}

void Game::adViewDidFailToReceiveAdWithError(const std::string &name, const std::string &msg) {
    showText(StringUtils::format("%s\nname=%s,\nmsg=%s", __FUNCTION__, name.c_str(), msg.c_str()));
}

void Game::adViewWillPresentScreen(const std::string &name) {
    showText(StringUtils::format("%s name=%s", __FUNCTION__, name.c_str()));
}

// this one is called when an interstitial ad is dismissed
void Game::adViewWillDismissScreen(const std::string &name) {
    showText(StringUtils::format("%s name=%s", __FUNCTION__, name.c_str()));

    if (name == REVIVE_AD_NAME || name == GAME_OVER_AD_NAME || !UserDefault::getInstance()->getBoolForKey(NO_ADS_PURCHASED_KEY, false)) {
        // cache revive ads regardless as they are an option regardless of whether
        // the user purchased no-ads
        // (gameover can be used as a substitute revive ad, so cache that too)
        // otherwise skip loading ads if the user purchased the no-ads item
        PluginAdMob::cache(name);
    }
    if (revivedCharacter && name == GAME_OVER_AD_NAME) {
        // this means the user wants to revive but we couldn't load a rewarded video ad
        // so we loaded the gameover ad instead
        // we get a different dismissed callback for the fullscreen ad, so we handle reviving here
        // instead of below
        revive();
    }
}

// this one is called when a rewarded video ad is dismissed
void Game::adViewDidDismissScreen(const std::string &name) {
    showText(StringUtils::format("%s name=%s", __FUNCTION__, name.c_str()));
    
    if (name == REVIVE_AD_NAME) {
        // revivedCharacter will be true if the user got the reward (separate callback)
        // in that case, we call revive()
        if (revivedCharacter) {
            CCLOG("watched video ad, reviving character");
            revive();
        } else {
            CCLOG("did not complete video ad, restarting game");
            // in this case, -reward hasn't been called, so the user exited the ad early
            // behave as if the replay button was pressed
            hideReviveMenu();
            replay();
        }
    }
}

void Game::adViewWillLeaveApplication(const std::string &name) {
    showText(StringUtils::format("%s name=%s", __FUNCTION__, name.c_str()));
}

void Game::reward(const std::string &name, const std::string &currency, double amount) {
    // when the user gets the reward, this function will be called before adViewDidDismissScreen
    // so if -adViewDidDismissScreen is called without this, we know the user exited early, and
    // we should go back to the main screen
    
    if (name == REVIVE_AD_NAME) {
        revivedCharacter = true;
    }
}

#endif

Scene* Game::createScene() {
    return Game::create();
}

// end listener

Game::~Game() {
    releaseGameObjects();
    delete physicsLoader;
}

void Game::retainGameObjects() {
    // retain items that will be added/removed from the screen throughout the game
    // this prevents them from being autoreleased, which would crash the game when they're added back
    player->retain();
    
    // since switching from an add/remove system to an opacity system (fade in/out), I no longer have
    // to retain buttons, etc. here
}

void Game::releaseGameObjects() {
    // release objects retained in init()
    
    // buttons will be released by cocos2d as long as their reference count is 1 (not greater)
    
    /*
    player->release();
    
    title->release();
    scoresButton->release();
    pauseButton->release();
    gameOverMenu->release();
    
    scoresButton->release();
    shareButton->release();
    
    replayButton->release();
    */
}

// HWAlert demo
//void alertClickedButtonAtIndex(HWAlert* alert, long index) {
//    CCLOG("alert clicked button at index (%p, %ld)", alert, index);
//}

bool Game::init() {
    
#if SCREENSHOT_MODE
#warning DEBUG DEBUG DEBUG
    UserDefault* defaults = UserDefault::getInstance();
    defaults->setIntegerForKey(HIGH_SCORE_KEY, 0);
    defaults->setBoolForKey(NO_ADS_PURCHASED_KEY, true);
    defaults->flush();
#endif
    
    
    // setting the height to origin.y + 100 (for example) instead of just 100 is
    // necessary because on devices where the visible height is less due to cropping,
    // origin.y will be a positive value and will be necessary to ensure a consistent layout

    // HWAlert demo
    // auto alert = HWAlert::create("HWAlert Test", "Let's see if the new code works on the latest iOS.", "Cancel", "OK", "It works!", NULL);
//    auto alert = HWAlert::create("HWAlert Test", "Let's see if the new code works on the latest Android.", "It works!", NULL);
//    HWAlertCallback callback = alertClickedButtonAtIndex;
//    alert->setCallback(callback);
//    alert->setCallback([&](HWAlert* alert, long index) {
//        CCLOG("alert clicked button at index (%p, %ld)", alert, index);
//    });
//    alert->show();
    
    // super init first
    if ( !Scene::init() ) {
        return false;
    }
    
    // Sprites would normally be loaded here (moved to SplashScreen)
    
    // There were collision and contact issues with the PhysicsEditor so I left it out
    
    // PhysicsEditor
//    physicsLoader = new PhysicsLoader();
//    physicsLoader->addShapesWithFile("bodies.plist");

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    CCLOG("got visible size (%.2f x %.2f) origin (%.2f, %.2f)", visibleSize.width, visibleSize.height, origin.x, origin.y);
    
#ifdef SDKBOX_ENABLED
    // PluginAdMob::cache("home");
    // PluginAdMob::cache("gameover");

    PluginAdMob::setListener(this);

    PluginSdkboxPlay::setListener(this);
    PluginSdkboxPlay::signin();
    
    sdkbox::IAP::setListener(this);
    sdkbox::IAP::getProducts();
#endif
    
    UserDefault* defaults = UserDefault::getInstance();
    highScore = defaults->getIntegerForKey(HIGH_SCORE_KEY, 0);
    
    // detect touches
    addEvents();
    
    // configure cameras
    
    auto s = Director::getInstance()->getWinSize();
    
    // cameras MUST be added in the order in which they should be rendered
    
    fixedCamera = Camera::getDefaultCamera();
    
    backgroundCamera = Camera::createOrthographic(s.width, s.height, 1, 1000);
    backgroundCamera->setPosition3D(Vec3(0, 0, 100));
    backgroundCamera->setCameraFlag(CameraFlag::USER1);
    this->addChild(backgroundCamera);
    
    trackingCamera = Camera::createOrthographic(s.width, s.height, 1, 1000);
    trackingCamera->setPosition3D(Vec3(0, 0, 100));
    trackingCamera->setCameraFlag(CameraFlag::USER2);
    this->addChild(trackingCamera);
    
    // exit menu (debug)
    /*
    // close icon
    auto closeItem = MenuItemImage::create("CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(Game::menuCloseCallback, this));
    closeItem->setPosition(origin.x + visibleSize.width - closeItem->getContentSize().width/2,
                           origin.y + closeItem->getContentSize().height/2);
    
    
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(0, 50); // above ad
    this->addChild(menu, 1);
    */
    
    // preload compressed sfx audio files
    // For SOME reason, preloading the audio files causes the SFX_PING sound to be crackled when it plays.
    // This is fixed by not preloading the audio files.
    /*
    SimpleAudioEngine* audioEngine = SimpleAudioEngine::getInstance();
    audioEngine->preloadEffect(SFX_JUMP);
    audioEngine->preloadEffect(SFX_PING);
    audioEngine->preloadEffect(SFX_GAMEOVER);
    */

    soundEnabled = UserDefault::getInstance()->getBoolForKey(SOUND_KEY, SOUND_DEFAULT);
    setSoundEnabled(soundEnabled);
    
    soundButton = GameButton::create();
    soundButton->loadTextureNormal(soundEnabled ? Images::sound : Images::mute, Widget::TextureResType::PLIST);
    soundButton->setPosition(Vec2(origin.x + 50, origin.y + visibleSize.height - 50));
    soundButton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            CCLOG("sound clicked");
            
            UserDefault* defaults = UserDefault::getInstance();
            soundEnabled = defaults->getBoolForKey(SOUND_KEY, SOUND_DEFAULT);
            soundEnabled = !soundEnabled;
            setSoundEnabled(soundEnabled);
            soundButton->loadTextureNormal(soundEnabled ? Images::sound : Images::mute, Widget::TextureResType::PLIST);
            defaults->setBoolForKey(SOUND_KEY, soundEnabled);
            defaults->flush();
        }
    });
    soundButton->hide();
    
    // pause button
    pauseButton = GameButton::create();
    pauseButton->loadTextureNormal(Images::pause, Widget::TextureResType::PLIST);
    pauseButton->cocos2d::Node::setPosition(origin.x + visibleSize.width - 50,
                                            origin.y + visibleSize.height - 50);
    pauseButton->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            CCLOG("pause clicked");
            
            // Crash Test
            // std::vector<std::string> crashTest = std::vector<std::string>();
            // crashTest.at(1);
            
            // pauseGame() is also called when there is an interruption, so adding this check
            // in the button code instead of within pauseGame() ensures that the game isn't resumed
            // if there are 2 interruptions in a row.
            // pauseGame() can only pause the game, while only the button code can resume it.
            if (gameMode == GameMode::PLAYING) {
                pauseGame();
            }
#if SCREENSHOT_MODE
            else if (gameMode == GameMode::PAUSED) {
                gameMode = GameMode::PLAYING;
                startAnimating();
            }
#endif
        }
    });
    pauseButton->hide();
    
    // buttons
    
    scoresButton = GameButton::create();
    scoresButton->loadTextureNormal(Images::scores, Widget::TextureResType::PLIST);
    scoresButton->cocos2d::Node::setPosition(origin.x + visibleSize.width * 0.2f,
                                             origin.y + visibleSize.height * 0.75f);
    scoresButton->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            CCLOG("scores clicked");
            
#           ifdef SDKBOX_ENABLED
            
            // if Google Play is not available, HWAvailability will show an error alert and return false
            if (HWAvailability::isLeaderboardAvailable()) {
                if (PluginSdkboxPlay::isSignedIn()) {
                    PluginSdkboxPlay::showLeaderboard(LEADERBOARD_NAME);
                } else {
                    waitingForLeaderboard = true;
                    PluginSdkboxPlay::signin();
                }
            }
            
#           endif
        }
    });
    this->addChild(scoresButton, 1);
    
    shareButton = GameButton::create();
    shareButton->loadTextureNormal(Images::share, Widget::TextureResType::PLIST);
    shareButton->setPosition(Vec2(origin.x + visibleSize.width * 0.8f, origin.y + visibleSize.height * 0.75f));
    shareButton->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            CCLOG("share clicked");
            
#       ifdef SDKBOX_ENABLED
            sdkbox::SocialShareInfo info;
            info.text = StringUtils::format("Check out the game Skater Bro!");
            //info.title = "Skater Bro";
            //info.image = "path/to/image"
            info.link = "http://onelink.to/skaterbro";
            info.showDialog = true; //if you want share with dialog，set the value true
            
            //sdkbox::SocialPlatform::Platform_Select will show platforms list, let user select which platform want to share
            //sdkbox::SocialPlatform::Platform_Twitter will share with twitter directly
            //sdkbox::SocialPlatform::Platform_Facebook will share with facebook directly
            
            // The Facebook SDK does not allow you to set the text of a post.
            // https://stackoverflow.com/questions/23460513/ios-facebook-sdk-post-text-status-post-simple-text-on-wall
            
            info.platform = sdkbox::SocialPlatform::Platform_Select;
            sdkbox::PluginShare::share(info);
            // sdkbox::PluginShare::nativeShare(info);
#       endif
        }
    });
    this->addChild(shareButton, 1);
    
    noAdsButton = GameButton::create();
    noAdsButton->loadTextureNormal(Images::noAds, Widget::TextureResType::PLIST);
    noAdsButton->setPosition(Vec2(origin.x + visibleSize.width * 0.5f,
                                  origin.y + visibleSize.height * 0.73f));
    noAdsButton->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            CCLOG("no ads clicked");
#ifdef SDKBOX_ENABLED
            // purchaseButtonTitle must be a std::string so it won't go out of scope until it is used in the alert
            // (if it was a char* and assigned by myString.c_str(), myString would immediately pass out of scope in
            // its "if" clause and the char* would then point to garbage
            std::string purchaseButtonTitle = "Purchase";
            if (noAdsPrice.size() > 0) {
                purchaseButtonTitle = StringUtils::format("Purchase %s", noAdsPrice.c_str());
            }

            // "Restore Purchase" has been shortened to "Restore" so it will fit in the alert
            // on the Note 3
            auto alert = HWAlert::create("No Ads",
                            "This will remove banner ads and fullscreen ads in the app. "
                            "You will still be able to watch video ads to revive your player if you want to.",
                            "Cancel", purchaseButtonTitle.c_str(), "Restore", NULL);
            alert->setCallback([&](HWAlert* alert, long buttonIndex) {
                if (buttonIndex == 1) {
                    // purchase
                    CCLOG("purchase");
                    sdkbox::IAP::purchase(NO_ADS_IAP_NAME);
                } else if (buttonIndex == 2) {
                    // restore purchase
                    CCLOG("restore purchase");
                    didRestorePurchases = false;
                    sdkbox::IAP::restore();
                }
            });
            alert->show();
#endif
        }
    });
    this->addChild(noAdsButton, 1);
    
    // game over
    
    pauseMenu = FadeSprite::createWithSpriteFrameName(Images::revive); // revive sprite is a larger menu
    pauseMenu->setPosition(180, 320);
    pauseMenu->setOpacity(0);
    pauseMenu->setCascadeOpacityEnabled(true);
    
    Sprite* playerIcon = Images::createPlayerIconSprite();
    playerIcon->setPosition(150, 350);
    pauseMenu->addChild(playerIcon);
    
    Scoreboard* pausedLabel = Scoreboard::create(60, 1, false);
    pausedLabel->setString("PAUSED");
    pausedLabel->setPosition(150, 225);
    pauseMenu->addChild(pausedLabel);
    
    resumeButton = GameButton::create();
    resumeButton->loadTextureNormal(Images::play, Widget::TextureResType::PLIST);
    resumeButton->cocos2d::Node::setPosition(150, 75);
    resumeButton->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            CCLOG("resume clicked");
            
            if (tutorialShown) {
                tutorialSprite->fadeIn(this, FADE_TIME);
            } else {
                scoreboard->runAction(FadeIn::create(FADE_TIME));
            }
            
            pauseButton->fadeIn(this, FADE_TIME);
            
            pauseMenu->fadeOut(FADE_TIME);
            resumeButton->hide();
            
            gameMode = GameMode::PLAYING;
            startAnimating();
        }
    });
    resumeButton->hide();
    pauseMenu->addChild(resumeButton);
    
    reviveMenu = FadeSprite::createWithSpriteFrameName(Images::revive);
    reviveMenu->setPosition(180, 320);
    reviveMenu->setOpacity(0);
    reviveMenu->setCascadeOpacityEnabled(true); // fade in children as well
    
    auto watchToContinue = Images::createWatch_to_continueSprite();
    watchToContinue->setPosition(150, 230);
    reviveMenu->addChild(watchToContinue);
    
    gameOverMenu = FadeSprite::createWithSpriteFrameName(Images::gameover);
    gameOverMenu->setPosition(180, 320);
    gameOverMenu->setOpacity(0);
    gameOverMenu->setCascadeOpacityEnabled(true); // fade in children as well
    
    trophy = Sprite::create();
    trophy->setPosition(90, 240);
    gameOverMenu->addChild(trophy);
    
    trophyRevive = Sprite::create();
    trophyRevive->setPosition(90, 360);
    reviveMenu->addChild(trophyRevive);
    
    finalScore = Scoreboard::create(75, 1, false);
    finalScore->setPosition(210, 240);
    gameOverMenu->addChild(finalScore);
    
    finalScoreRevive = Scoreboard::create(75, 1, false);
    finalScoreRevive->setPosition(210, 380);
    reviveMenu->addChild(finalScoreRevive);
    
    finalScoreMessage = Scoreboard::create(30, 1, false);
    finalScoreMessage->setPosition(150, 140);
    gameOverMenu->addChild(finalScoreMessage);
    
    finalScoreMessageRevive = Scoreboard::create(30, 1, false);
    finalScoreMessageRevive->setPosition(210, 315);
    reviveMenu->addChild(finalScoreMessageRevive);
    
#ifdef SDKBOX_ENABLED
    // set listener
    sdkbox::PluginShare::setListener(this);
#endif
    
    shareScoreButton = GameButton::create();
    shareScoreButton->loadTextureNormal(Images::share, Widget::TextureResType::PLIST);
    shareScoreButton->setPosition(Vec2(80, 60));
    shareScoreButton->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            CCLOG("share clicked");
            
#       ifdef SDKBOX_ENABLED
            sdkbox::SocialShareInfo info;
            info.text = StringUtils::format("I got a score of %d on Skater Bro!", highScore);
            //info.title = "Skater Bro";
            //info.image = "path/to/image"
            info.link = "http://onelink.to/skaterbro";
            info.showDialog = true; //if you want share with dialog，set the value true
            
            //sdkbox::SocialPlatform::Platform_Select will show platforms list, let user select which platform want to share
            //sdkbox::SocialPlatform::Platform_Twitter will share with twitter directly
            //sdkbox::SocialPlatform::Platform_Facebook will share with facebook directly
            
            // The Facebook SDK does not allow you to set the text of a post.
            // https://stackoverflow.com/questions/23460513/ios-facebook-sdk-post-text-status-post-simple-text-on-wall
            
            info.platform = sdkbox::SocialPlatform::Platform_Select;
            sdkbox::PluginShare::share(info);
            // sdkbox::PluginShare::nativeShare(info);
#       endif
        }
    });
    shareScoreButton->hide();
    gameOverMenu->addChild(shareScoreButton, 1);
    
    replayButton = GameButton::create();
    replayButton->loadTextureNormal(Images::replay, Widget::TextureResType::PLIST);
    replayButton->setPosition(Vec2(300 - 80, 60));
    // MUST capture by value because otherwise visibleSize will be 0
    replayButton->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            // originally I showed an interstitial whenever the gameover menu was shown
            // (the revive menu only got an interstitial after the user pressed the replay button),
            // however for consistency and fairness I've decided the user shoudl only see
            // an interstitial when they want to replay the game, so I moved it here instead
            showInterstitial();

            gameOverMenu->fadeOut(FADE_TIME);
            shareScoreButton->hide();
            replayButton->hide();
            
            replay();
        }
    });
    replayButton->hide();
    gameOverMenu->addChild(replayButton);
    
    videoAdButton = GameButton::create();
    videoAdButton->loadTextureNormal(Images::video, Widget::TextureResType::PLIST);
    videoAdButton->setPosition(Vec2(150, 140));
    videoAdButton->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
#ifdef SDKBOX_ENABLED
            if (reviveAdLoaded) {
                PluginAdMob::show(REVIVE_AD_NAME);
            } else if (gameoverAdLoaded) {
                // if we couldn't load a video ad, show the fullscreen game over ad instead
                // and set revivedCharacter = true to revive them instead of getting the rewarded video callback
                PluginAdMob::show(GAME_OVER_AD_NAME);
                revivedCharacter = true;
            } else {
                // couldn't get a rewarded video ad or a fullscreen ad
                // but since the user requested it, revive them anyway
                revive();
                revivedCharacter = true;
            }
#else
            revive();
            revivedCharacter = true;
#endif
        }
    });
    videoAdButton->hide();
    reviveMenu->addChild(videoAdButton);
    
    replayButtonRevive = GameButton::create();
    replayButtonRevive->loadTextureNormal(Images::replay, Widget::TextureResType::PLIST);
    replayButtonRevive->setPosition(Vec2(150, 60));
    // MUST capture by value because otherwise visibleSize will be 0
    replayButtonRevive->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            // the interstitial is delayed while the user decides whether to revive their character
            // if they decide not to revive by pressing this button, show the interstitial at this time
            showInterstitial();
            
            hideReviveMenu();
            replay();
        }
    });
    replayButtonRevive->hide();
    reviveMenu->addChild(replayButtonRevive);
    
    // privacy policy required in the app since we use the advertising ID
    auto p2Label = Label::createWithTTF("privacy policy", "fonts/8bit.ttf", 15);
    auto p2menuItem = MenuItemLabel::create(p2Label, [&](Ref* sender) {
        CCLOG("privacy policy clicked");
#ifdef SDKBOX_ENABLED
        HWURLOpen::openURL(PRIVACY_POLICY_URL);
#endif
    });
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // having the privacy policy in the app is only required on Android
    privacyPolicyMenu = Menu::create(p2menuItem, NULL);
    privacyPolicyMenu->setPosition(Vec2(180, visibleSize.height * 0.125f));
    privacyPolicyMenu->alignItemsHorizontallyWithPadding(10);
    this->addChild(privacyPolicyMenu);
#endif
    
    
    // -- game over
    
    /*
    title = Images::createTitleSprite();
    title->setPosition(180, origin.y + visibleSize.height * 0.78f);
    this->addChild(title);
    */
    
    title = FadeSprite::createWithSpriteFrameName(Images::title);
    title->setPosition(180, origin.y + visibleSize.height * 0.9f);
    this->addChild(title);
    
    // score
    scoreboard = Scoreboard::create(72, 2, true);
    scoreboard->setPosition(180, origin.y + visibleSize.height - 100);
    scoreboard->setOpacity(0);
    this->addChild(scoreboard);
    
    // background
    background = Background::create(50);
    // background exists in its own layer with its own camera
    // this allows us to easily loop the background without worrying about the player's x and y offset
    background->setCameraMask((unsigned short)CameraFlag::USER1);
    this->addChild(background, 0);
    
    tutorialSprite = FadeSprite::createWithSpriteFrameName(Images::tap);
    tutorialSprite->setPosition(180, origin.y + visibleSize.height * 0.26);
    this->addChild(tutorialSprite);
    
    // physics setup
    
    physicsManager = PhysicsManager::getInstance();
    _world = physicsManager->getWorld();
    _world->SetContactListener(this);
    
    // player
    
    player = Images::createPlayerSprite();
    player->setCameraMask((unsigned short)CameraFlag::USER2); // tracking camera
    this->addChild(player);
    
    b2BodyDef playerBodyDef;
    playerBodyDef.type = b2_dynamicBody;
    playerBodyDef.userData = player;
    _playerBody = _world->CreateBody(&playerBodyDef);
    
    // physicsLoader->addFixturesToBody(_playerBody, Images::player);
    
    // this keeps the skateboarder vertical but jitters if you hit the corner of a rectangle
    b2CircleShape circleShape;
    circleShape.m_radius = 15.0f/PTM_RATIO;
    
//    b2PolygonShape boxShape;
//    boxShape.SetAsBox(PLAYER_W/2/PTM_RATIO, PLAYER_W/2/PTM_RATIO);
    
    b2FixtureDef playerFixtureDef;
    playerFixtureDef.shape = &circleShape;
    playerFixtureDef.density = 0.1f;
    playerFixtureDef.friction = 0.0f;
    playerFixtureDef.restitution = 0.0f; // do not bounce
    _playerBody->CreateFixture(&playerFixtureDef);
    
    b2CircleShape weightShape;
    weightShape.m_radius = PLAYER_W/4.0f/PTM_RATIO;
    circleShape.m_p = b2Vec2(PLAYER_W/4.0f/PTM_RATIO, PLAYER_W/4.0f/PTM_RATIO);
    
    // if the player tilts, bring it back to an upright position
    b2FixtureDef playerBottomWeight;
    playerBottomWeight.shape = &weightShape;
    playerBottomWeight.density = 3.6f;
    playerBottomWeight.friction = 0.0f;
    playerBottomWeight.restitution = 0.0f;
    _playerBody->CreateFixture(&playerBottomWeight);
    
    // game over menu test
//    gameMode = GameMode::GAME_OVER;
//    gameOver();
//    finalScore->setString("432");
//    finalScoreMessage->setString("NEW HIGH SCORE!");

    waitingForLeaderboard = false;
    
    // initial config
    gameMode = GameMode::MENU;
    
    // set up game
    setupGame();
    
    retainGameObjects();
    
    // ads
    gameoverAdLoaded = false;
    gameoverAdCounter = 0;
    
#if SCREENSHOT_MODE
    
    // press space to jump (so the mouse doesn't interfere with screen recordings)
    // press "r" to replay
    // press the "f" key to take a screenshot!
    
    auto obKeyBackListener = EventListenerKeyboard::create();
    obKeyBackListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_SPACE) {
            event->stopPropagation(); // stop propagation for this event
            
            handleTouchDown(nullptr);
        } else if (keyCode == EventKeyboard::KeyCode::KEY_R) {
            event->stopPropagation();
            
            gameOverMenu->fadeOut(FADE_TIME);
            shareScoreButton->hide();
            replayButton->hide();
            
            replay();
            
        } else if (keyCode == EventKeyboard::KeyCode::KEY_F) {
            event->stopPropagation(); // stop propagation for this event
            
            static int index = 1;
            utils::captureScreen(NULL, StringUtils::format("s%d.png", index++));
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(obKeyBackListener, this);
    
#endif
    
    return true;
}

void Game::revive() {
    CCLOG("revive");
    
    hideReviveMenu();
    
    // do not show tutorial on replay

    scoreboard->runAction(FadeIn::create(FADE_TIME));
    
//    score = 0;
//    updateScore();
    
    soundButton->fadeIn(this, FADE_TIME);
    pauseButton->fadeIn(this, FADE_TIME);
    
    setupGame();
    gameMode = GameMode::PLAYING;
}

void Game::hideReviveMenu() {
    reviveMenu->fadeOut(FADE_TIME);
    videoAdButton->hide();
    replayButtonRevive->hide();
}

void Game::replay() {
    gameMode = GameMode::MENU;
    
    title->fadeIn(this, FADE_TIME);
    // add the scores and share button before the no-ads button
    // so the no-ads button will be on top
    scoresButton->fadeIn(this, FADE_TIME);
    shareButton->fadeIn(this, FADE_TIME);
    noAdsButton->fadeIn(this, FADE_TIME);
    tutorialSprite->fadeIn(this, FADE_TIME);
    
    if (privacyPolicyMenu) {
        privacyPolicyMenu->runAction(FadeIn::create(FADE_TIME));
        privacyPolicyMenu->setEnabled(true);
    }
    
    setupGame();
}

void Game::setSoundEnabled(const bool enabled) {
    SimpleAudioEngine::getInstance()->setEffectsVolume(enabled ? 1 : 0);
}

void Game::setupGame() {
    // From the box2d manual:
    // Once a body is destroyed, its next pointer becomes invalid.
    // So the call to b2Body::GetNext() will return garbage. The solution to this is to copy the next pointer
    // before destroying the body.
    
    b2Body* node = _world->GetBodyList();
    while (node) {
        b2Body* b = node;
        node = node->GetNext();
        
        if (b != _playerBody && b->GetUserData() != nullptr) {
            Platform* platform = static_cast<Platform*>(b->GetUserData());
            if (platform) {
                this->removeChild(platform);
                _world->DestroyBody(b);
            }
        }
    }
    
    makeRunway();
    
    // place player on the runway
    float baselineYPixels = 0 + TILE_SIZE_H/2.0f + player->getContentSize().height/2.0f;
    baselineY = baselineYPixels/PTM_RATIO;
    _playerBody->SetLinearVelocity(b2Vec2(PLAYER_VELOCITY, 0));
    _playerBody->SetTransform(b2Vec2(MENU_PLAYER_X_START/PTM_RATIO, baselineY), 0);
    
    // show player, score, pause button if hidden after game ended
    player->setOpacity(255);
    
    // adjust camera so it's in the correct position BEFORE animation starts
    // (otherwise the player will appear to "fall" for a moment until the camera adjusts)
    moveCamera(0);
    
    startAnimating();
    touchingPlatform = true;
}

void Game::pauseGame() {
    if (gameMode == GameMode::PLAYING) {
        gameMode = GameMode::PAUSED;
        stopAnimating();
        
#if (SCREENSHOT_MODE == 0)
        if (tutorialShown) {
            tutorialSprite->fadeOut(FADE_TIME);
        } else {
            scoreboard->runAction(FadeOut::create(FADE_TIME));
        }
        
        pauseButton->fadeOut(FADE_TIME);
        pauseMenu->fadeIn(this, FADE_TIME);
        resumeButton->show();
#endif
    }
}

void Game::startAnimating() {
    background->startAnimating();
    schedule(schedule_selector(Game::step));
}

void Game::stopAnimating() {
    background->stopAnimating();
    unschedule(schedule_selector(Game::step));
}

bool Game::validateTouch(const Touch* touch) {
    Vec2 p = touch->getLocation();
    Rect rect = this->getBoundingBox();
    
    if (rect.containsPoint(p)) {
        return true; // to indicate that we have consumed it
    }
    
    return false; // we did not consume this event, pass through
}

void Game::addEvents() {
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [&](Touch* touch, Event* event) {
        if (validateTouch(touch)) {
            handleTouchDown(touch);
            return true;
        }
        return false;
    };
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
}

void Game::handleTouchDown(Touch* touch) {
    CCLOG("touch down");
    
    if (gameMode == GameMode::MENU) {
        CCLOG("first touch -> play");
        
        // If the player pressed the scores button but wasn't signed in,
        // the leaderboard will be shown after sign-in is complete.
        // This cancels showing the leaderboard so it won't interrupt the game.
        waitingForLeaderboard = false;
        
        score = 0;
        updateScore();
        
        soundButton->fadeIn(this, FADE_TIME);
        pauseButton->fadeIn(this, FADE_TIME);
        
        tutorialShown = true;
        
        title->fadeOut(FADE_TIME);
        scoresButton->fadeOut(FADE_TIME);
        noAdsButton->fadeOut(FADE_TIME);
        shareButton->fadeOut(FADE_TIME);
        
        if (privacyPolicyMenu) {
            privacyPolicyMenu->runAction(FadeOut::create(FADE_TIME));
            privacyPolicyMenu->setEnabled(false);
        }
        
        gameMode = GameMode::PLAYING;
    }
    
    if (gameMode == GameMode::GAME_OVER || gameMode == GameMode::PAUSED) {
        // return on GAME_OVER
        // otherwise, if the user touches after the game is over,
        // the player will spontaneously jump when the next game starts
        return;
    }
    
    tapped = true;
    tapTime = 0;
    
    // It is NOT reliable to check the contact list of the physics body;
    // instead, we use the BeginContact() and EndContact() methods to track whether the player can jump
    
    // if (_playerBody->GetContactList() != nullptr && _playerBody->GetContactList()->contact->IsTouching()) {
    
    if (touchingPlatform) {
        CCLOG("touching -> begin jump");
        beginJump();
    }
}

void Game::beginJump() {
    if (tutorialShown) {
        tutorialSprite->fadeOut(FADE_TIME);
        tutorialShown = false;
        
        scoreboard->runAction(FadeIn::create(FADE_TIME));
    }
    
    _playerBody->SetLinearVelocity(b2Vec2(_playerBody->GetLinearVelocity().x, JUMP_VELOCITY));
    if (soundEnabled) {
        SimpleAudioEngine::getInstance()->playEffect(SFX_JUMP);
    }
}

void Game::BeginContact(b2Contact* contact) {
    CCLOG("begin contact @ x = %.2f", _playerBody->GetPosition().x * PTM_RATIO);
    touchingPlatform = true;
    
    if (gameMode != GameMode::PLAYING) {
        // ignore runway touchdown
        // does not set baseline (that's done manually)
        // nor increment the score
        return;
    }
    
    
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    
    b2Body* platformBody = nullptr;
    
    if (bodyA == _playerBody) {
        platformBody = bodyB;
    }
    else if (bodyB == _playerBody) {
        platformBody = bodyA;
    }
    
    if (platformBody) {
        Platform* platform = static_cast<Platform *>(platformBody->GetUserData());
        
        if (!platform->getScored()) {
            platform->setScored(true);
            
            CCLOG("scored on platform %p", platform);
            
            ++score;
            updateScore();
            
            if (soundEnabled) {
                SimpleAudioEngine::getInstance()->playEffect(SFX_PING);
            }
            
            baselineY = _playerBody->GetPosition().y;
            CCLOG("updating baseline y to %.1f (player body y is %.1f)", baselineY, _playerBody->GetPosition().y);
        }
    }
    
    if (tapped && tapTime <= TAP_TIME_FUDGE_FACTOR) {
        tapped = false;
        beginJump();
    }
}

void Game::EndContact(b2Contact* contact) {
    CCLOG("end contact @ x = %.2f", _playerBody->GetPosition().x * PTM_RATIO);
    touchingPlatform = false;
}

void Game::updateScore() {
    scoreboard->setString(StringUtils::format("%d", score));
}

void Game::step(float dt) {
    // two-stepping here causes serious jitter in the game!! fixed by only allowing _world to step once per game step!
    // what I learned from this is:
    
    // If the device is under high CPU load for a moment and skips a frame, there will be jitter no matter what,
    // so the accumulator provides no benefit.
    // But using an accumulator will cause jitter when the device does NOT skip a frame each time
    // the accumulator either:
    // (a) crosses the physics time step threshold, which will happen consistently throughout the game if it is
    //     running slightly faster than the design frequency (60 Hz) as the remainders add up, or
    // (b) misses the physics time step threshold, which will happen consistently throughout the game if it is
    //     running slightly slower than the design frequency (60 Hz)


    // UPDATE: box2d required a constant time step, otherwise there is a noticeable tremor in the
    // game movement. For this reason, we use a constant time step equal to 1/(target framerate).
    // This can be achieved by calling Director::getInstance()->getAnimationInterval().


    // This is what I was doing before (DO NOT DO THIS!)
//    _accumulator += dt;
//    if (_accumulator >= PHYSICS_TIME_STEP) {
//        _accumulator -= PHYSICS_TIME_STEP;
//
//        /* minimum position and velocity iterations that result in proper physics; box2d manual recommends 8,3 */
//        _world->Step(PHYSICS_TIME_STEP, 8, 3);
//    }
    
    // This is what I'm doing now (notice dt is used as the time step)
    /* minimum position and velocity iterations that result in proper physics; box2d manual recommends 8,3 */
    _world->Step(Director::getInstance()->getAnimationInterval(), 8, 3);
    
    if (dt >= 1.0/35) {
        // game has been throttled down to 30 fps by the OS
        // usually from high CPU usage by ads
        // we don't want to make the game easier by having it run in slow motion
        // if we single-stepped above,
        // so we step again in this case (but we don't accumulate the value which would cause jitter)
        // *** we check dt >= 1.0/35 instead of 1.0/30 to leave a margin of error in case the actual framerate
        // is slightly higher than 30fps, which would otherwise cause jitter from inconsistent two-stepping here
        _world->Step(Director::getInstance()->getAnimationInterval(), 8, 3);
    }
    
    // don't reset the player's velocity while they're falling
    // in case they hit a wall we don't want to drive them into it
    if (_playerBody->GetLinearVelocity().y >= 0) {
        _playerBody->SetLinearVelocity(b2Vec2(PLAYER_VELOCITY, _playerBody->GetLinearVelocity().y));
    }
    
    // tap fudge factor
    tapTime += dt;
    
    // render
    
    b2Body* node = _world->GetBodyList();
    while (node) {
        b2Body* b = node;
        node = node->GetNext();
        
        if (b->GetUserData() != nullptr) {
            Sprite *sprite = (Sprite *)b->GetUserData();
            sprite->setPosition(b->GetPosition().x * PTM_RATIO,
                                b->GetPosition().y * PTM_RATIO);
            
            // because the player is modeled in the physics engine with a circle,
            // we can ignore its rotation angle without being inconsistent with the game physics
            
            if (b != _playerBody) {
                sprite->setRotation(-CC_RADIANS_TO_DEGREES(b->GetAngle()));
            }
        }
    }
    
    // track cameras
    moveCamera(dt);
    
    removePassedTiles();
    
    if (gameMode == GameMode::MENU || (gameMode == GameMode::PLAYING && tutorialShown)) {
        float playerXPixels = _playerBody->GetPosition().x * PTM_RATIO;
        while (playerXPixels > MENU_PLAYER_X_START + MENU_PLAYER_X_DELTA) {
            playerXPixels -= MENU_PLAYER_X_DELTA;
        }

        _playerBody->SetTransform(b2Vec2(playerXPixels/PTM_RATIO, _playerBody->GetPosition().y), 0);
    }
    else if (gameMode == GameMode::PLAYING) {
        generateFutureTiles();
    }
}

void Game::moveCamera(const float dt) {

    // compare Y coordinates in the physics engine WITHOUT converting them to pixels
    // this can result in rounding error that incorrectly assumes the player has fallen
    // when in reality they are less than one pixel below the baseline due to rounding error

    float playerX = _playerBody->GetPosition().x;
    float playerY = _playerBody->GetPosition().y;

    float playerXPixels = playerX * PTM_RATIO;
    float playerYPixels = playerY * PTM_RATIO;
    float baselineYPixels = baselineY * PTM_RATIO;
    
    // ensure that the window scaling doesn't result in the player being out of view
    float adjustedXOffset = MAX(Director::getInstance()->getVisibleOrigin().x - PLAYER_W, PLAYER_X_OFFSET);

    trackingCamera->setPosition(playerXPixels-adjustedXOffset, playerYPixels-320);

    // ensure the platforms are completely off the screen before stopping the game animation
    if (playerYPixels <= baselineYPixels-400) {
        gameOver();
    }
}

void Game::gameOver() {
    CCLOG("*** GAME OVER ***");
    
    gameMode = GameMode::GAME_OVER;
    
    // stop animation
    stopAnimating();
    
    if (soundEnabled) {
        SimpleAudioEngine::getInstance()->playEffect(SFX_GAMEOVER);
    }
    
    // if the player taps while falling, they will jump as soon as the next game starts
    // if this isn't reset to false
    tapped = false;
    
    // hide player, pause button & score
    scoreboard->runAction(FadeOut::create(FADE_TIME));
    
    soundButton->fadeOut(FADE_TIME);
    pauseButton->fadeOut(FADE_TIME);
    
    player->setOpacity(0);
    
    if (score > highScore) {
        highScore = score;
        
        UserDefault* defaults = UserDefault::getInstance();
        defaults->setIntegerForKey(HIGH_SCORE_KEY, score);
        defaults->flush();
        
        finalScoreMessage->setString("NEW HIGH SCORE!");
        
        // if it was the high score, fade in the game over menu (has sharing button for score)
        
        showGameOverMenu();
    }
    else {
        finalScoreMessageRevive->setString(StringUtils::format("BEST: %d", highScore));
        finalScoreMessage->setString(StringUtils::format("BEST: %d", highScore));
        
        // if it wasn't the high score, fade in the revive menu (no sharing button for score)
        
        if (revivedCharacter) {
            // if the player has already revived their character,
            // they cannot revive them a second time
            revivedCharacter = false;
            showGameOverMenu();
        } else {
            if (score > 0) {
                // allow player to revive their character once if their score was more than 0
                showReviveMenu();
            } else {
                // if the player's score was 0, do not give them the option to "revive"
                showGameOverMenu();
            }
        }
    }
    
#ifdef SDKBOX_ENABLED
    PluginSdkboxPlay::submitScore(LEADERBOARD_NAME, score);
#endif
}

void Game::showInterstitial() {
#ifdef SDKBOX_ENABLED
    // skip loading ads if the user purchased the no-ads item
    if (!UserDefault::getInstance()->getBoolForKey(NO_ADS_PURCHASED_KEY, false)) {
        if (gameoverAdLoaded && ++gameoverAdCounter >= INTERSTITIAL_FREQUENCY) {
            gameoverAdCounter = 0;
            
            CCLOG("showing gameover interstitial");
            PluginAdMob::show(GAME_OVER_AD_NAME);
        }
    }
#endif
}

void Game::showReviveMenu() {
    finalScoreRevive->setString(StringUtils::format("%d", score));
    
    std::vector<std::string> achievements;
    std::string spriteFrameName = Images::trophyNone;
    
    if (score >= 10) {
        achievements.push_back(ACH_BRONZE);
        spriteFrameName = Images::trophyBronze;
    }
    if (score >= 20) {
        achievements.push_back(ACH_SILVER);
        spriteFrameName = Images::trophySilver;
    }
    if (score >= 30) {
        achievements.push_back(ACH_GOLD);
        spriteFrameName = Images::trophyGold;
    }
    if (score >= 50) {
        achievements.push_back(ACH_PLATINUM);
        spriteFrameName = Images::trophyPlatinum;
    }
    
#ifdef SDKBOX_ENABLED
    for (std::string ach: achievements) {
        PluginSdkboxPlay::unlockAchievement(ach);
    }
#endif
    
    trophyRevive->setSpriteFrame(spriteFrameName);
    
    reviveMenu->fadeIn(this, FADE_TIME);
    videoAdButton->show();
    replayButtonRevive->show();
}

void Game::showGameOverMenu() {
    scoreboard->setString("");
    finalScore->setString(StringUtils::format("%d", score));
    
    std::vector<std::string> achievements;
    std::string spriteFrameName = Images::trophyNone;
    
    if (score >= 10) {
        achievements.push_back(ACH_BRONZE);
        spriteFrameName = Images::trophyBronze;
    }
    if (score >= 20) {
        achievements.push_back(ACH_SILVER);
        spriteFrameName = Images::trophySilver;
    }
    if (score >= 30) {
        achievements.push_back(ACH_GOLD);
        spriteFrameName = Images::trophyGold;
    }
    if (score >= 50) {
        achievements.push_back(ACH_PLATINUM);
        spriteFrameName = Images::trophyPlatinum;
    }
    
#ifdef SDKBOX_ENABLED
    for (std::string ach: achievements) {
        PluginSdkboxPlay::unlockAchievement(ach);
    }
#endif
    
    trophy->setSpriteFrame(spriteFrameName);
    
    gameOverMenu->fadeIn(this, FADE_TIME);
    shareScoreButton->show();
    replayButton->show();
}

/***************************************************************      GROUND GENERATOR      ***********************************************************************/

void Game::makeRunway() {
    // reset fill coordinates
    fillX = -TILE_SIZE_W; // left-side tile lies off camera
    fillY = 0;

    // fill at least the design resolution width x 2
    // take the max in case the infinite scroll in the menu needs a longer runway
    // add 1 for rounding error
    int doubleDesignResolutionWidth = 360*2/TILE_SIZE_W+1;
    int menuInfiniteScrollWidth     = (MENU_PLAYER_X_START + MENU_PLAYER_X_DELTA + 360)/TILE_SIZE_W+1;

    addPlaneSegment(MAX(doubleDesignResolutionWidth, menuInfiniteScrollWidth), true); // scored = true, hopping on the runway won't award a point
}

void Game::removePassedTiles() {
    // remove passed tiles
    int cutoffX = trackingCamera->getPositionX() - Director::getInstance()->getVisibleSize().width/2.0f;
    
    // See note above -- when a body is destroyed, its next pointer becomes invalid.
    // Therefore, the next pointer must be backed up before destroying the body
    
    b2Body* node = _world->GetBodyList();
    while (node) {
        b2Body* b = node;
        node = node->GetNext();
        
        if (b != _playerBody && b->GetUserData() != nullptr) {
            Platform *platform = static_cast<Platform *>(b->GetUserData());
            if (platform) {
                float x = b->GetPosition().x * PTM_RATIO;
                if (x + platform->getContentSize().width/2.0f < cutoffX) {
                    CCLOG("reaping platform %p", b);
                    this->removeChild(platform);
                    _world->DestroyBody(b);
                }
            }
        }
    }
}

void Game::generateFutureTiles() {
    // generate future tiles
    int fillLimitX = trackingCamera->getPositionX() + Director::getInstance()->getVisibleSize().width * 2;
    
    while (fillX < fillLimitX) {
        CCLOG("making tiles");
        
        // add tiles in front as needed
        // keep 2 screens' worth of tiles loaded at all times
        
        // 1 was too cruel
        int len = random(2, 8);
        
        addPlaneSegment(len, false);
    }
}

void Game::addPlaneSegment(const int len, const bool scored) {
    
    // box2d handles the physics for the platform;
    // the platform is a Node that contains multiple Sprites for images
    
    Platform *platform = Platform::create();
    platform->setPosition(fillX + TILE_SIZE_W*len/2.0f, fillY);
    platform->setCameraMask((unsigned short)CameraFlag::USER2); // tracking camera
    platform->setContentSize(Size(TILE_SIZE_W * len, TILE_SIZE_H));
    platform->setScored(scored);
    
    float boxX = -TILE_SIZE_W * len / 2;
    
    for (int i = 0; i < len; i++) {
        std::string image = Images::ground;
        
        if (len == 1) {
            image = Images::groundSole; // single tile
        } else if (i == 0) {
            image = Images::groundLeft; // beginning tile
        } else if (i == len-1) {
            image = Images::groundRight; // end tile
        }
        
        Sprite* box = Sprite::createWithSpriteFrameName(image);
        box->setCameraMask((unsigned short)CameraFlag::USER2); // tracking camera
        // box->setContentSize(Size(TILE_SIZE_W, TILE_SIZE_H));
        box->setPosition(boxX, 0);
        platform->addChild(box);
        
        /*
        if (random(0, 20) == 0) {
            Sprite* longboard = Images::createLongboard2Sprite();
            longboard->setCameraMask((unsigned short)CameraFlag::USER2);
            longboard->setPosition(boxX, 55);
            platform->addChild(longboard);
        }
        */
        
        boxX += TILE_SIZE_W;
    }
    
    b2BodyDef boxBodyDef;
    boxBodyDef.type = b2_staticBody;
    boxBodyDef.position.Set( (fillX + TILE_SIZE_W * len / 2)/PTM_RATIO, fillY/PTM_RATIO);
    boxBodyDef.userData = platform;
    b2Body *boxBody = _world->CreateBody(&boxBodyDef);
    
    // box takes half-width and half-height as parameters
    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(TILE_SIZE_W * len/2/PTM_RATIO, TILE_SIZE_H/2/PTM_RATIO);
    
    b2FixtureDef boxFixtureDef;
    boxFixtureDef.friction = 0.0f; // slide forever
    boxFixtureDef.shape = &polygonShape;
    boxFixtureDef.restitution = 0.0f; // do not bounce
    boxBody->CreateFixture(&boxFixtureDef);
    
    //this->tiles.pushBack(platform);
    this->addChild(platform);
    
    float lastPlatformStartX = fillX;
    float lastPlatformEndX = fillX + len*TILE_SIZE_W;
    
    // add gap in between platforms for jump
    
    switch (random(0, 1 /* 2 */)) {
        case 0: {
            // 4w is pretty much the limit and Lamis thought that was too hard
            // fillX += random(TILE_SIZE_W * 1, 6);
            
            // no increase in fillY (gap in platform)
            // to make sure the game is possible to win, some part of the next platform (on the same level)
            // must be 243px
            
            fillX = random(MAX(lastPlatformStartX + LEVEL_JUMP_DISTANCE, lastPlatformEndX + 1*TILE_SIZE_W), lastPlatformEndX + LEVEL_JUMP_DISTANCE);
            break;
        }
        case 1: {
            // 4w is pretty much the limit and Lamis thought that was too hard
            fillX = lastPlatformEndX + random(TILE_SIZE_W * 23, TILE_SIZE_W * 32) / 10.0f; // 2.3 - 3.2
            fillY += random(160, 190);
            break;
        }
            // lower platform
            /*
        case 2: {
            fillX = lastPlatformEndX + random(0, TILE_SIZE_W * 2);
            fillY -= random(160, 190);
            break;
        }
             */
    }
}

void Game::menuCloseCallback(Ref* pSender) {
    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
    
#   if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#   endif
    
    /* To navigate back to native iOS screen(if present) without quitting the application, do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    // EventCustom customEndEvent("game_scene_close_event");
    // _eventDispatcher->dispatchEvent(&customEndEvent);
}
