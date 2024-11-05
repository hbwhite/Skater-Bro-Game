// ---------------------------------------
// Sprite definitions for Images
// Generated with TexturePacker 4.11.1
//
// https://www.codeandweb.com/texturepacker
// 
// $TexturePacker:SmartUpdate:08bbb347f9c0b4f56fc2216197b9b6e3:309249c1c4485989f9a0b07cb54a510b:eaf397b18f878cd52290672a5a69ba8d$
// ---------------------------------------

#ifndef __IMAGES_H__
#define __IMAGES_H__

#include "cocos2d.h"

namespace TexturePacker
{

class Images
{
public:
    /**
     * Add sprite frames contained in theImages sheet to
     * the SpriteFrameCache.
     */
    static void addSpriteFramesToCache();

    /**
     * Remove sprite frames contained in theImages sheet from
     * the SpriteFrameCache.
     */
    static void removeSpriteFramesFromCache();

    // ---------------------
    // sprite name constants
    // ---------------------
    static const std::string background_01;
    static const std::string background_02;
    static const std::string background_03;
    static const std::string gameover;
    static const std::string ground;
    static const std::string groundLeft;
    static const std::string groundRight;
    static const std::string groundSole;
    static const std::string mute;
    static const std::string noAds;
    static const std::string pause;
    static const std::string play;
    static const std::string player;
    static const std::string playerIcon;
    static const std::string replay;
    static const std::string revive;
    static const std::string scores;
    static const std::string share;
    static const std::string sound;
    static const std::string tap;
    static const std::string title;
    static const std::string trophyBronze;
    static const std::string trophyGold;
    static const std::string trophyNone;
    static const std::string trophyPlatinum;
    static const std::string trophySilver;
    static const std::string video;
    static const std::string watch_to_continue;

    // --------------
    // Sprite objects
    // --------------
    static cocos2d::Sprite *createBackground_01Sprite();
    static cocos2d::Sprite *createBackground_02Sprite();
    static cocos2d::Sprite *createBackground_03Sprite();
    static cocos2d::Sprite *createGameoverSprite();
    static cocos2d::Sprite *createGroundSprite();
    static cocos2d::Sprite *createGroundLeftSprite();
    static cocos2d::Sprite *createGroundRightSprite();
    static cocos2d::Sprite *createGroundSoleSprite();
    static cocos2d::Sprite *createMuteSprite();
    static cocos2d::Sprite *createNoAdsSprite();
    static cocos2d::Sprite *createPauseSprite();
    static cocos2d::Sprite *createPlaySprite();
    static cocos2d::Sprite *createPlayerSprite();
    static cocos2d::Sprite *createPlayerIconSprite();
    static cocos2d::Sprite *createReplaySprite();
    static cocos2d::Sprite *createReviveSprite();
    static cocos2d::Sprite *createScoresSprite();
    static cocos2d::Sprite *createShareSprite();
    static cocos2d::Sprite *createSoundSprite();
    static cocos2d::Sprite *createTapSprite();
    static cocos2d::Sprite *createTitleSprite();
    static cocos2d::Sprite *createTrophyBronzeSprite();
    static cocos2d::Sprite *createTrophyGoldSprite();
    static cocos2d::Sprite *createTrophyNoneSprite();
    static cocos2d::Sprite *createTrophyPlatinumSprite();
    static cocos2d::Sprite *createTrophySilverSprite();
    static cocos2d::Sprite *createVideoSprite();
    static cocos2d::Sprite *createWatch_to_continueSprite();

    // ----------------
    // animation frames
    // ----------------
    static cocos2d::Vector<cocos2d::SpriteFrame*> getBACKGROUNDAnimationFrames();

    // ---------------
    // animate actions
    // ---------------
    static cocos2d::Animate *createBACKGROUNDAnimateAction(float delay, unsigned int loops = 1);

}; // class

}; // namespace

#endif // __IMAGES_H__
