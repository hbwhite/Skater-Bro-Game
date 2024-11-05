//
//  Scoreboard.hpp
//  chocolate
//
//  Created by Harrison White on 1/21/18.
//

#ifndef Scoreboard_h
#define Scoreboard_h

#include "cocos2d.h"

USING_NS_CC;

class Scoreboard : public Node {
public:
    
    static Scoreboard* create(const int fontSize, const int resolution, const bool isometricShadow) {
        Scoreboard *pRet = new(std::nothrow) Scoreboard();
        if (pRet && pRet->init(fontSize, resolution, isometricShadow)) {
            pRet->autorelease();
            return pRet;
        }
        else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }
    
    void setString(const std::string& str) {
        label->setString(str);
    }
    
private:
    Label* label;
    
    bool init(const int fontSize, const int resolution, const bool isometricShadow) {
        if ( !Node::init() ) {
            return false;
        }
        
        this->setCascadeOpacityEnabled(true); // when the scoreboard object's opacity is reduced, fade the label
        this->setColor(Color3B::BLACK);
        
        label = Label::createWithTTF("", "fonts/8bit.ttf", fontSize*resolution);
        label->setScale(1.0f/resolution); // 2x font size and 0.5 scale, otherwise border will be blurry on retina
        
        label->setAlignment(TextHAlignment::CENTER);
        label->setTextColor(Color4B::WHITE);
        label->setPosition(0, 0);
        
        if (fontSize < 72) {
            label->enableOutline(Color4B::BLACK, 2.0f);
        }
        else {
            label->enableOutline(Color4B::BLACK, 4.0f); // larger numbers cause glitches on this font
        }
        
        // isometric shadow is required to fill internal gaps (such as the hole in "9")
//        if (isometricShadow) {
            label->enableShadow(Color4B::BLACK, Size(fontSize/36.0f, -fontSize/36.0f), fontSize/36.0f); // 72pt -> 2
//        }
        
        this->addChild(label, 1);
        
        return true;
    }
};

#endif /* Scoreboard_h */
