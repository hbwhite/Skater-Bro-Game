//
//  GroundGenerator.cpp
//  chocolate
//
//  Created by Harrison White on 1/21/18.
//

#include "GroundGenerator.hpp"
#include "Player.hpp"
#include "Scoreboard.hpp"
#include "PhysicsManager.hpp"

// physics
#include <Box2D/Box2D.h>

static const int    TILE_SIZE_W         = 32;
static const int    TILE_SIZE_H         = 32;

bool GroundGenerator::init() {
    // super init
    if (!Node::init()) {
        return false;
    }
    
    physicsManager = PhysicsManager::getInstance();
    
    score = 0;
    
    // build runway
    
    fillX = 0;
    fillY = 100;
    
#warning there are issues because the tiles are centered, so incrementing x doesn't help
    
    addPlaneSegment(55);
    fillY += random(140, 210);
    fillX += 4 * TILE_SIZE_W;
    
//    for (int i = 0; i < 15; i++) {
//        addPlaneSegment(random(1, 16));
//        fillY += random(140, 210);
//        fillX += 4 * TILE_SIZE_W;
//    }
    
    return true;
}

void GroundGenerator::removePassedTiles() {
    // remove passed tiles
    int cutoffX = Camera::getDefaultCamera()->getPositionX() - Director::getInstance()->getVisibleSize().width;
    
    
#warning automatically add/remove tiles (current todo item) -- need to find the width of the platform before deciding whether to remove it
    
    b2World *world = physicsManager->getWorld();
    
    for (b2Body *b = world->GetBodyList(); b; b=b->GetNext()) {
        if (b->GetUserData() != NULL) {
            Sprite *platform = (Sprite *)b->GetUserData();
            
            float x = b->GetPosition().x * PTM_RATIO;
            float y = b->GetPosition().y * PTM_RATIO;
            
            if (x + platform->getContentSize().width < cutoffX) {
                
                printf("reaping platform %p\n", b);

                // should be unnecessary?
//                for (Sprite* tile : platform->getChildren()) {
//                    box->removeChild(tile, true);
//                }
                
                this->removeChild(platform, true);
                world->DestroyBody(b);
            }
        }
    }
}

void GroundGenerator::generateFutureTiles() {
    // generate future tiles
    int fillLimitX = Camera::getDefaultCamera()->getPositionX() + Director::getInstance()->getVisibleSize().width * 2;
    
    while (fillX < fillLimitX) {
        printf("making tiles\n");
        
        // add tiles in front as needed
        // keep 2 screens' worth of tiles loaded at all times
        
        int len = random(1, 16);
        
        addPlaneSegment(len);
        
        // add gap in between platforms for jump
        fillX += TILE_SIZE_W * 4;
        fillY += random(140, 210);
    }
}

void GroundGenerator::addPlaneSegment(const int len) {
    
    // box2d handles the physics for the platform;
    // the platform is a Node that contains multiple Sprites for images
    
    Node *platform = Node::create();
    platform->setContentSize(Size(TILE_SIZE_W * len, TILE_SIZE_H));
    
    float boxX = -TILE_SIZE_W * len / 2;
    
    for (int i = 0; i < len; i++) {
        std::string image = "ground.png";
        
        if (len == 1) {
            image = "ground4.png";
        }
        else if (i == 0) {
            image = "ground2.png";
        }
        else if (i == len-1) {
            image = "ground3.png";
        }
        
        Sprite *box = Sprite::create(image);
        box->setContentSize(Size(TILE_SIZE_W, TILE_SIZE_H));
        box->setPosition(boxX, 0);
        platform->addChild(box);
        
        boxX += TILE_SIZE_W;
    }
    
    b2World *world = physicsManager->getWorld();
    
    b2BodyDef boxBodyDef;
    boxBodyDef.type = b2_staticBody;
    boxBodyDef.position.Set( (fillX + TILE_SIZE_W * len / 2)/PTM_RATIO, fillY/PTM_RATIO);
    boxBodyDef.userData = platform;
    b2Body *boxBody = world->CreateBody(&boxBodyDef);
    
    // box takes half-width and half-height as parameters
    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(TILE_SIZE_W * len/2/PTM_RATIO, TILE_SIZE_H/2/PTM_RATIO);
    
    b2FixtureDef boxFixtureDef;
    boxFixtureDef.friction = 0.0f; // slide forever
    boxFixtureDef.shape = &polygonShape;
    boxBody->CreateFixture(&boxFixtureDef);
    
    //this->tiles.pushBack(platform);
    this->addChild(platform);
    
    fillX += TILE_SIZE_W * len;
    
}
