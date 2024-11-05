//
//  Platform.hpp
//  chocolate
//
//  Created by Harrison White on 1/26/18.
//
//

#ifndef Platform_h
#define Platform_h

#import "cocos2d.h"

USING_NS_CC;

class Platform : public Node {
public:
    CREATE_FUNC(Platform);
    
    void setScored(const bool scored) {
        this->scored = scored;
    }
    bool getScored() {
        return scored;
    }
private:
    bool scored;
    
    bool init() {
        if ( !Node::init() ) {
            return false;
        }
        
        // setup
        scored = false;
        
        return true;
    }
};

#endif /* Platform_h */
