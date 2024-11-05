//
//  Background.hpp
//  catwalk-mobile
//
//  Created by Harrison White on 12/27/17.
//

#ifndef Background_hpp
#define Background_hpp

#include "cocos2d.h"
#include "BackgroundComposite.hpp"
#include <string>

USING_NS_CC;

// overlap to prevent flickering 1px vertical bar in between
// the 2 backgrounds due to rounding error
static const int    BACKGROUND_OVERLAP_MARGIN = 2;

class Background : public Node {

public:

    // aspect ratio of designer screen should be preserved in image
    static Background* create(const unsigned int speed) {

        Background *pRet = new(std::nothrow) Background();

        if (pRet && pRet->init(speed)) {
            pRet->autorelease();
            return pRet;
        }
        else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }
    
    void setSpeed(const unsigned int speed) {
        this->speed = speed;
    }

    void stopAnimating() {
        unschedule(schedule_selector(Background::update));
    }

    void startAnimating() {
        schedule(schedule_selector(Background::update));
    }

private:

    BackgroundComposite* background1;
    BackgroundComposite* background2;
    unsigned int speed;

    bool init(const unsigned int speed) {

        if ( !Node::init() ) {
            return false;
        }

        // value may be negative to represent travel in -x direction
        this->speed = speed;

        setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        setPosition(0, 0);

        // backgrounds

        // change background size in order to stretch image for devices
        // with screens larger than the background itself
        
        // this is just a convenience function for development; if it works
        // on the designer screen, it will work on the device when it is scaled

        // uses SpriteFrame cache (to load plain files, use plain create())
        background1 = BackgroundComposite::create();
        background2 = BackgroundComposite::create();

        background1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        background2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

        // overlap to prevent flickering 1px vertical bar in between
        // the 2 backgrounds due to rounding error
        background1->setPosition(Vec2::ZERO);
        background2->setPosition(Vec2(background1->getContentSize().width - BACKGROUND_OVERLAP_MARGIN, 0));

        this->addChild(background1);
        this->addChild(background2);

        return true;
    }

    // x and y are a ratio between 0 and 1 representing how far the player has moved
    void update(float dt) {

        float dx = dt * speed;
        
        // move backgrounds first so dx doesn't multiply if the below loop iterates multiple times
        background1->setPositionX(background1->getPositionX() - dx);
        background2->setPositionX(background2->getPositionX() - dx);
        
        float w = background1->getContentSize().width;

        auto leftBackground = background1;
        auto rightBackground = background2;

        if (background2->getPositionX() < background1->getPositionX()) {
            leftBackground = background2;
            rightBackground = background1;
        }

        // swap backgrounds if left one has moved off the screen
        // do this several times if necessary to "catch up" (in case there's ridiculous lag)
        while (leftBackground->getPositionX() <= -w) {

            // overlap to prevent flickering vertical bar in between
            // the 2 backgrounds due to rounding error

            // positioning the right background relative to the left (and not just relative to itself)
            // prevents rounding error from driving the two backgrounds apart over many swap iterations

            leftBackground->setPositionX(rightBackground->getPositionX() + w - BACKGROUND_OVERLAP_MARGIN);

            if (leftBackground == background1) {
                leftBackground = background2;
                rightBackground = background1;
            }
            else {
                leftBackground = background1;
                rightBackground = background2;
            }
        }
    }
};

#endif /* Background_hpp */
