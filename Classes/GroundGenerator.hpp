//
//  GroundGenerator.hpp
//  catwalk
//
//  Created by Harrison White on 12/27/17.
//

#ifndef GroundGenerator_hpp
#define GroundGenerator_hpp

#include "cocos2d.h"

USING_NS_CC;

class PhysicsManager;

class GroundGenerator : public Node {
    
public:
    CREATE_FUNC(GroundGenerator);
    void tap();
    void move(float dt);
    
    void startAnimating();
    void stopAnimating();
    
    void removePassedTiles();
    void generateFutureTiles();
    
private:
    typedef enum {
        kTilePositionLeft,
        kTilePositionMiddle,
        kTilePositionRight,
        kTilePositionLone
    } kTilePosition;
    
    PhysicsManager *physicsManager;
    
    // score
    int score;
    
    // where to start filling new tiles
    float fillX;
    float fillY;
    
    bool init();
    void beginFreefall(const float startVelocity);
    
    // add segments
    void addPlaneSegment(const int len);
};

#endif /* GroundGenerator_hpp */
