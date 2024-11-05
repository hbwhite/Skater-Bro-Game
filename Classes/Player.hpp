//
//  Player.h
//  catwalk
//
//  Created by Harrison White on 12/28/17.
//

#ifndef Player_h
#define Player_h

#include "cocos2d.h"

USING_NS_CC;

/*
static const Size   PLAYER_SIZE(58, 72);
static const int    PLAYER_FRAMES_LEN                   =  4;
static const char*  PLAYER_FRAMES[PLAYER_FRAMES_LEN] = {
    "character-1.png",
    "character-2.png",
    "character-3.png",
    "character-2.png",
};
static const float  PLAYER_FRAME_TIME                   = 0.2f;

static const int    PLAYER_FRAME_W                      = 117;
static const int    PLAYER_FRAME_H                      = 144;
*/

class Player : public Sprite {
public:
    static Player* getInstance() {
        // see readme for magic static singletons
        static Player* instance = Player::create("player.png");
        return instance;
    }

private:
    static Player* create(const std::string& fileName) {
        Player *pRet = new(std::nothrow) Player();
        if (pRet && pRet->initWithFile(fileName)) {
            pRet->autorelease();
            return pRet;
        }
        else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

    bool initWithFile(const std::string& fileName) {
        
        // super init
        if ( !Sprite::createWithSpriteFrameName(/* PLAYER_FRAMES[0] */ fileName) ) {
            return false;
        }
        
        setContentSize(Size(29, 36));
        
        /*
        // when using SpriteFrames, do not set content size;
        // it will be scaled and set automatically based on the director's contentScaleFactor
         
        Vector<SpriteFrame*> frames;

        for (int i = 0; i < PLAYER_FRAMES_LEN; i++) {
            auto* frame = SpriteFrame::create(PLAYER_FRAMES[i], Rect(0, 0, PLAYER_FRAME_W, PLAYER_FRAME_H));
            frames.pushBack(frame);
        }

        Animation* animation = Animation::createWithSpriteFrames(frames, PLAYER_FRAME_TIME);
        Animate* animate = Animate::create(animation);
        RepeatForever* repeat = RepeatForever::create(animate);
        this->runAction(repeat);
        */
        
        return true;
    }
};

#endif /* Player_h */
