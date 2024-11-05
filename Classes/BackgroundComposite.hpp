//
//  BackgroundComposite.hpp
//  skaterbro
//
//  Created by Harrison White on 3/24/18.
//

#ifndef BackgroundComposite_h
#define BackgroundComposite_h

#include "cocos2d.h"

USING_NS_CC;

static const int BG_COUNT           = 3;
static const int BG_INCREMENT       = 360;
static const int BG_OVERLAP_MARGIN  = 0;
static const int BG_TOTAL_WIDTH     = 1066;

class BackgroundComposite: public Node {
public:
    CREATE_FUNC(BackgroundComposite);
    
private:
    bool init() override {
        if (!Node::init()) {
            return false;
        }
        
        this->setContentSize(Size(BG_TOTAL_WIDTH - (BG_COUNT * BG_OVERLAP_MARGIN), 640));
        
        for (int i = 0; i < BG_COUNT; i++) {
            Sprite* bg = Sprite::createWithSpriteFrameName(StringUtils::format("background_%02d", i+1));
            bg->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            bg->setPosition(i * (BG_INCREMENT - BG_OVERLAP_MARGIN), 0);
            this->addChild(bg);
        }
        
        return true;
    }
};

#endif /* BackgroundComposite_h */
