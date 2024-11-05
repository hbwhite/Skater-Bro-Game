#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

// gui
#include "ui/CocosGUI.h"

// physics
#include <Box2D/Box2D.h>

// game center & ads
#ifdef SDKBOX_ENABLED
#include "pluginsdkboxplay/PluginSdkboxPlay.h"
#include "pluginshare/PluginShare.h"
#include "pluginadmob/PluginAdMob.h"
#include "pluginfacebook/PluginFacebook.h"
#include "pluginiap/PluginIAP.h"

#include "PlayAdapter.hpp"
#include "IAPAdapter.hpp"
#endif

USING_NS_CC;
using namespace ui;     // cocos2d::ui::

class Background;
class PhysicsManager;
class Scoreboard;
class PhysicsLoader;

#import "HWAlert.h"
#include "GameButton.hpp"

class Game : public Scene, public b2ContactListener
#ifdef SDKBOX_ENABLED
, public PlayAdapter
, public IAPAdapter
, public sdkbox::ShareListener
, public sdkbox::AdMobListener /* , public PlayAdapter */
#endif
{
public:
    
    enum class GameMode {
        MENU = 0,
        PLAYING,
        PAUSED,
        GAME_OVER
    };
    
    ~Game();
    
    // ads
    
#ifdef SDKBOX_ENABLED
    // play adapter
    void onConnectionStatusChanged(int status) override;

    // no ads
    void grantNoAds(bool restored);
    
    // iap adapter
    void onSuccess(sdkbox::Product const& p) override;
    void onFailure(sdkbox::Product const& p, const std::string &msg) override;
    void onCanceled(sdkbox::Product const& p) override;
    void onRestored(sdkbox::Product const& p) override;
    void onProductRequestSuccess(std::vector<sdkbox::Product> const &products) override;
    void onProductRequestFailure(const std::string &msg) override;
    void onRestoreComplete(bool ok, const std::string &msg) override;
    
    void onShareState(const sdkbox::SocialShareResponse& response) override;
    
    /*
    void onLoadGameData(const SavedGameData* savedData, const std::string& error) override;
    void onConnectionStatusChanged(int status) override;
    void onMyScore(const std::string& leaderboard_name, int time_span, int collection_type, long score) override;
    void getHighScore(float dt);
    */
    
    void adViewDidReceiveAd(const std::string &name) override;
    void adViewDidFailToReceiveAdWithError(const std::string &name, const std::string &msg) override;
    void adViewWillPresentScreen(const std::string &name) override;
    void adViewWillDismissScreen(const std::string &name) override;
    void adViewDidDismissScreen(const std::string &name) override;
    void adViewWillLeaveApplication(const std::string &name) override;
    void reward(const std::string &name, const std::string &currency, double amount) override;
#endif

    // fetched from the server
    std::string noAdsPrice;

    // game mode is primarily used for camera tricks, but is also used for transitions
    // update gameMode, then call startGame()/endGame()/etc.
    GameMode gameMode;
    
    // the fixed camera isn't available until after the scene is initialized
    Camera* fixedCamera;
    Camera* trackingCamera;
    Camera* backgroundCamera;

    // when the user pressed the leaderboard button but is still waiting to sign in to game center
    // this will load the leaderboard as soon as sign-in is complete
    bool waitingForLeaderboard;
    
    // detect fall
    float baselineY;
    bool baselineSet;
    
    // used to jump if the player taps just before they touch down
    bool tapped;
    float tapTime;
    
    // sound
    bool soundEnabled;
    
    // game elements
    
    GameButton* scoresButton;
    GameButton* noAdsButton;
    
    GameButton* shareButton;
    GameButton* shareScoreButton;
    GameButton* replayButton;
    
    GameButton* videoAdButton;
    GameButton* replayButtonRevive;
    
    Background* background;
    GameButton* soundButton;
    GameButton* pauseButton;
    GameButton* resumeButton;
    Menu* privacyPolicyMenu;

    FadeSprite* tutorialSprite;
    bool tutorialShown;
    bool revivedCharacter;
    
    bool didRestorePurchases;
    
    Sprite* player;
    PhysicsBody* playerBody;
    
    Scoreboard* scoreboard;
    FadeSprite* pauseMenu;
    FadeSprite* reviveMenu;
    FadeSprite* gameOverMenu;
    
    Sprite* trophy;
    FadeSprite* title;
    Scoreboard* finalScore;
    Scoreboard* finalScoreMessage;
    
    Sprite* trophyRevive;
    Scoreboard* finalScoreRevive;
    Scoreboard* finalScoreMessageRevive;
    
    // where to start filling new tiles
    float fillX;
    float fillY;
    
    // physics
    PhysicsManager *physicsManager;
    PhysicsLoader* physicsLoader;
    
    b2World* _world;
    b2Body* _playerBody;
    float _accumulator;
    
    // contact
    bool touchingPlatform;
    
    
    CREATE_FUNC(Game);
    static Scene* createScene();
    void retainGameObjects();
    void releaseGameObjects();
    
    void loadGameData(float dt);
    virtual bool init() override;
    
    void setupGame();
    void pauseGame();
    void startAnimating();
    void stopAnimating();
    
    void run(float dt);
    void updateScore();
    void revive();
    void hideReviveMenu();
    void replay();
    void setSoundEnabled(const bool enabled);
    bool validateTouch(const Touch* touch);
    void addEvents();
    void handleTouchDown(Touch* touch);
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
    void step(float dt);
    void moveCamera(const float dt);
    void beginJump();
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    void gameOver();
    void showInterstitial();
    void showReviveMenu();
    void showGameOverMenu();
    
    // ground generator
    void makeRunway();
    void removePassedTiles();
    void generateFutureTiles();
    void addPlaneSegment(const int len, const bool scored);
    
    // ads
    bool gameoverAdLoaded;
    int gameoverAdCounter;
    bool reviveAdLoaded;
    
    // score
    int score;
    int highScore;
    
    
    
    
    
    
    void test(float dt);
};

#endif // __HELLOWORLD_SCENE_H__
