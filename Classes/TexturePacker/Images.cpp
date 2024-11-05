// ---------------------------------------
// Sprite definitions for Images
// Generated with TexturePacker 4.11.1
//
// https://www.codeandweb.com/texturepacker
// ---------------------------------------

#include "Images.hpp"

USING_NS_CC;

namespace TexturePacker
{

void Images::addSpriteFramesToCache()
{
    SpriteFrameCache *cache = SpriteFrameCache::getInstance();

    // load at least one texture
    cache->addSpriteFramesWithFile("images-1.plist");

    // the number of multipack files varies with the resolution
    // but we don't have information which search paths are set 
    // so we can only try to load the ones we find
    FileUtils *fileUtils = FileUtils::getInstance();
    bool restoreNotify = fileUtils->isPopupNotify();
    fileUtils->setPopupNotify(false); // disable warnings
    if(fileUtils->fullPathForFilename("images-2.plist").size() > 0)
    {
        cache->addSpriteFramesWithFile("images-2.plist");
    }
    if(fileUtils->fullPathForFilename("images-3.plist").size() > 0)
    {
        cache->addSpriteFramesWithFile("images-3.plist");
    }
    fileUtils->setPopupNotify(restoreNotify); // reset warnings
};

void Images::removeSpriteFramesFromCache()
{
    SpriteFrameCache *cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFramesFromFile("images-1.plist");
    cache->removeSpriteFramesFromFile("images-2.plist");
    cache->removeSpriteFramesFromFile("images-3.plist");
};


// ---------------------
// sprite name constants
// ---------------------
const std::string Images::background_01     = "background_01";
const std::string Images::background_02     = "background_02";
const std::string Images::background_03     = "background_03";
const std::string Images::gameover          = "gameover";
const std::string Images::ground            = "ground";
const std::string Images::groundLeft        = "ground-left";
const std::string Images::groundRight       = "ground-right";
const std::string Images::groundSole        = "ground-sole";
const std::string Images::mute              = "mute";
const std::string Images::noAds             = "no-ads";
const std::string Images::pause             = "pause";
const std::string Images::play              = "play";
const std::string Images::player            = "player";
const std::string Images::playerIcon        = "player-icon";
const std::string Images::replay            = "replay";
const std::string Images::revive            = "revive";
const std::string Images::scores            = "scores";
const std::string Images::share             = "share";
const std::string Images::sound             = "sound";
const std::string Images::tap               = "tap";
const std::string Images::title             = "title";
const std::string Images::trophyBronze      = "trophy-bronze";
const std::string Images::trophyGold        = "trophy-gold";
const std::string Images::trophyNone        = "trophy-none";
const std::string Images::trophyPlatinum    = "trophy-platinum";
const std::string Images::trophySilver      = "trophy-silver";
const std::string Images::video             = "video";
const std::string Images::watch_to_continue = "watch_to_continue";

// ---------------------------------------------------------
// convenience functions returing pointers to Sprite objects
// ---------------------------------------------------------
Sprite* Images::createBackground_01Sprite()
{
    return Sprite::createWithSpriteFrameName(background_01);
}

Sprite* Images::createBackground_02Sprite()
{
    return Sprite::createWithSpriteFrameName(background_02);
}

Sprite* Images::createBackground_03Sprite()
{
    return Sprite::createWithSpriteFrameName(background_03);
}

Sprite* Images::createGameoverSprite()
{
    return Sprite::createWithSpriteFrameName(gameover);
}

Sprite* Images::createGroundSprite()
{
    return Sprite::createWithSpriteFrameName(ground);
}

Sprite* Images::createGroundLeftSprite()
{
    return Sprite::createWithSpriteFrameName(groundLeft);
}

Sprite* Images::createGroundRightSprite()
{
    return Sprite::createWithSpriteFrameName(groundRight);
}

Sprite* Images::createGroundSoleSprite()
{
    return Sprite::createWithSpriteFrameName(groundSole);
}

Sprite* Images::createMuteSprite()
{
    return Sprite::createWithSpriteFrameName(mute);
}

Sprite* Images::createNoAdsSprite()
{
    return Sprite::createWithSpriteFrameName(noAds);
}

Sprite* Images::createPauseSprite()
{
    return Sprite::createWithSpriteFrameName(pause);
}

Sprite* Images::createPlaySprite()
{
    return Sprite::createWithSpriteFrameName(play);
}

Sprite* Images::createPlayerSprite()
{
    return Sprite::createWithSpriteFrameName(player);
}

Sprite* Images::createPlayerIconSprite()
{
    return Sprite::createWithSpriteFrameName(playerIcon);
}

Sprite* Images::createReplaySprite()
{
    return Sprite::createWithSpriteFrameName(replay);
}

Sprite* Images::createReviveSprite()
{
    return Sprite::createWithSpriteFrameName(revive);
}

Sprite* Images::createScoresSprite()
{
    return Sprite::createWithSpriteFrameName(scores);
}

Sprite* Images::createShareSprite()
{
    return Sprite::createWithSpriteFrameName(share);
}

Sprite* Images::createSoundSprite()
{
    return Sprite::createWithSpriteFrameName(sound);
}

Sprite* Images::createTapSprite()
{
    return Sprite::createWithSpriteFrameName(tap);
}

Sprite* Images::createTitleSprite()
{
    return Sprite::createWithSpriteFrameName(title);
}

Sprite* Images::createTrophyBronzeSprite()
{
    return Sprite::createWithSpriteFrameName(trophyBronze);
}

Sprite* Images::createTrophyGoldSprite()
{
    return Sprite::createWithSpriteFrameName(trophyGold);
}

Sprite* Images::createTrophyNoneSprite()
{
    return Sprite::createWithSpriteFrameName(trophyNone);
}

Sprite* Images::createTrophyPlatinumSprite()
{
    return Sprite::createWithSpriteFrameName(trophyPlatinum);
}

Sprite* Images::createTrophySilverSprite()
{
    return Sprite::createWithSpriteFrameName(trophySilver);
}

Sprite* Images::createVideoSprite()
{
    return Sprite::createWithSpriteFrameName(video);
}

Sprite* Images::createWatch_to_continueSprite()
{
    return Sprite::createWithSpriteFrameName(watch_to_continue);
}


// ------------------------------------------------
// convenience functions returning animation frames
// ------------------------------------------------

Vector<SpriteFrame*> Images::getBACKGROUNDAnimationFrames()
{
    SpriteFrameCache *frameCache = SpriteFrameCache::getInstance();
    Vector<SpriteFrame*> frames;
    frames.reserve(3);
    frames.pushBack(frameCache->getSpriteFrameByName(background_01));
    frames.pushBack(frameCache->getSpriteFrameByName(background_02));
    frames.pushBack(frameCache->getSpriteFrameByName(background_03));
    return frames;
}


// ----------
// animations
// ----------

Animate *Images::createBACKGROUNDAnimateAction(float delay, unsigned int loops)
{
    return Animate::create(Animation::createWithSpriteFrames(getBACKGROUNDAnimationFrames(), delay, loops));
}

}; // namespace

