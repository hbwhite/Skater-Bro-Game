//
//  PhysicsManager.h
//  chocolate
//
//  Created by Harrison White on 1/24/18.
//
//

#ifndef PhysicsManager_h
#define PhysicsManager_h

#include "Constants.hpp"

#include <Box2D/Box2D.h>

#define PTM_RATIO 32.0f

class PhysicsManager {
public:
    static PhysicsManager* getInstance() {
        static PhysicsManager* instance = new PhysicsManager();
        return instance;
    }
    
    b2World* getWorld() {
        return _world;
    }
    
private:
    b2World *_world;
    
    PhysicsManager() {
        // create a world
        b2Vec2 gravity = b2Vec2(0.0f, GRAVITY);
        _world = new b2World(gravity);
    }
    
    ~PhysicsManager() {
        delete _world;
        _world = NULL;
    }
};

#endif /* PhysicsManager_h */
