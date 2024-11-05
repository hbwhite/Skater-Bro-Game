//
//  GameButton.hpp
//  skaterbro
//
//  Created by Harrison White on 3/27/18.
//

#ifndef GameButton_h
#define GameButton_h

#include "cocos2d.h"
USING_NS_CC;

template<typename MyClass>
class Crossfade: public MyClass {
public:
    CREATE_FUNC(Crossfade<MyClass>);

    // retain nodes because they may not always be in the scene
    // which causes them to be autoreleased by cocos
    bool init() {
        if (MyClass::init()) {
            this->retain();
            return true;
        }
        return false;
    }

    // release this node because we retained it in init() above
    ~Crossfade() {
        this->release();
    }

    /* for when button is initially hidden */
    virtual void hide() {
        this->setOpacity(0);
    }
    /* for when the parent is fading in with cascade opacity */
    virtual void show() {
        this->setOpacity(255);
    }

    virtual void fadeIn(Node* parent, const float fadeTime) {
        this->setOpacity(0);
        parent->addChild(this);
        this->runAction(FadeIn::create(fadeTime));
    }
    virtual void fadeOut(const float fadeTime) {
        // this->setOpacity(255);
        this->runAction(Sequence::create(FadeOut::create(fadeTime), RemoveSelf::create(), NULL));
    }
};


// convenience subclasses

typedef Crossfade<cocos2d::ui::Button> CrossfadeButton;
class GameButton: public CrossfadeButton {
public:
    CREATE_FUNC(GameButton);

    void hide() override {
        CrossfadeButton::hide();
        this->setTouchEnabled(false);
    }

    void show() override {
        CrossfadeButton::show();
        this->setTouchEnabled(true);
    }

    void fadeIn(Node* parent, const float fadeTime) override {
        CrossfadeButton::fadeIn(parent, fadeTime);
        this->setTouchEnabled(true);
    }
    void fadeOut(const float fadeTime) override {
        CrossfadeButton::fadeOut(fadeTime);
        this->setTouchEnabled(false);
    }
};

class FadeSprite: public Crossfade<Sprite> {
public:
    static FadeSprite* createWithSpriteFrameName(const std::string& name) {
        FadeSprite *pRet = new(std::nothrow) FadeSprite();
        if (pRet->initWithSpriteFrameName(name)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

    // retain nodes because they may not always be in the scene
    // which causes them to be autoreleased by cocos

    // need to do this again here because the init() function is custom
    // (will be released automatically in ~Crossfade() in superclass)
    bool initWithSpriteFrameName(const std::string& name) {
        this->retain();
        return Sprite::initWithSpriteFrameName(name);
    }
};

#endif /* GameButton_h */
