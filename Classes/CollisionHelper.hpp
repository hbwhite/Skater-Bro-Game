//
//  CollisionHelper.hpp
//  catwalk
//
//  Created by Harrison White on 1/14/18.
//

#ifndef CollisionHelper_h
#define CollisionHelper_h

#import "cocos2d.h"

USING_NS_CC;

// debug

inline void debug(const Rect& r) {
    printf("[debug] rect (%.2f, %.2f, %.2f, %.2f)\n", r.origin.x, r.origin.y, r.size.width, r.size.height);
}

// ascending helper function
// LT = less than; true if x < y < z
// LTE = less than or equal to; true if x <= y <= z

inline bool ASC_LT(const float x, const float y, const float z) {
    return x < y && y < z;
}

inline bool ASC_LTE(const float x, const float y, const float z) {
    return x <= y && y <= z;
}

// detect whether player is standing within x/y boundaries of a given tile
// the player may technically contact the top of a tile, but if they aren't standing on that tile, it's irrelevant

inline bool WITHIN_X(const Rect& p, const Rect& t) {
    const float pl = p.origin.x;
    const float pr = p.origin.x + p.size.width;
    const float tl = t.origin.x;
    const float tr = t.origin.x + t.size.width;
    
    return pr > tl && pl < tr;
}

inline bool WITHIN_Y(const Rect& p, const Rect& t) {
    const float pt = p.origin.y + p.size.height;
    const float pb = p.origin.y;
    const float tt = t.origin.y + t.size.height;
    const float tb = t.origin.y;
    
    return pt > tb && pb < tt;
}

// straddle calculations
// LT = less than; true if overlapping

inline bool STR_LEFT_LT(const Rect& p, const Rect& t) {
    const float pl = p.origin.x;
    const float tl = t.origin.x;
    const float pr = p.origin.x + p.size.width;
    
    return ASC_LT(pl, tl, pr);
}

inline bool STR_RIGHT_LT(const Rect& p, const Rect& t) {
    const float pl = p.origin.x;
    const float tr = t.origin.x + t.size.width;
    const float pr = p.origin.x + p.size.width;
    
    return ASC_LT(pl, tr, pr);
}

inline bool STR_TOP_LT(const Rect& p, const Rect& t) {
    const float pt = p.origin.y + p.size.height;
    const float tt = t.origin.y + t.size.height;
    const float pb = p.origin.y;
    
    return ASC_LT(pb, tt, pt);
}

inline bool STR_BOTTOM_LT(const Rect& p, const Rect& t) {
    const float tt = t.origin.y + t.size.height;
    const float pt = p.origin.y + p.size.height;
    const float tb = t.origin.y;
    
    return ASC_LT(tb, pt, tt);
}

// LTE = less than or equal to; true if at least touching the edge

inline bool STR_LEFT_LTE(const Rect& p, const Rect& t) {
    const float pl = p.origin.x;
    const float tl = t.origin.x;
    const float pr = p.origin.x + p.size.width;
    
    return ASC_LTE(pl, tl, pr);
}

inline bool STR_RIGHT_LTE(const Rect& p, const Rect& t) {
    const float pl = p.origin.x;
    const float tr = t.origin.x + t.size.width;
    const float pr = p.origin.x + p.size.width;
    
    return ASC_LTE(pl, tr, pr);
}

inline bool STR_TOP_LTE(const Rect& p, const Rect& t) {
    const float pt = p.origin.y + p.size.height;
    const float tt = t.origin.y + t.size.height;
    const float pb = p.origin.y;
    
    return ASC_LTE(pb, tt, pt);
}

inline bool STR_BOTTOM_LTE(const Rect& p, const Rect& t) {
    const float tt = t.origin.y + t.size.height;
    const float pt = p.origin.y + p.size.height;
    const float tb = t.origin.y;
    
    return ASC_LTE(tb, pt, tt);
}

// example:
// xxx_TOP means the player hit the TOP of the block
// xxx_BOTTOM means the player hit the BOTTOM of the block

// for any given side to have contact, the player must
// be within the boundaries of the block,
// so WITHIN_X and WITHIN_Y are used

// collision calculations
// when the player would overlap the edge of a tile

inline bool COL_LEFT(const Rect& p, const Rect& t) {
    return STR_LEFT_LT(p, t) && WITHIN_Y(p, t);
}

inline bool COL_RIGHT(const Rect& p, const Rect& t) {
    return STR_RIGHT_LT(p, t) && WITHIN_Y(p, t);
}

inline bool COL_TOP(const Rect& p, const Rect& t) {
    return STR_TOP_LT(p, t) && WITHIN_X(p, t);
}

inline bool COL_BOTTOM(const Rect& p, const Rect& t) {
    return STR_BOTTOM_LT(p, t) && WITHIN_X(p, t);
}

// contact calculations
// when the player is just touching the edge of a tile

inline bool CON_LEFT(const Rect& p, const Rect& t) {
    return STR_LEFT_LTE(p, t) && WITHIN_Y(p, t);
}

inline bool CON_RIGHT(const Rect& p, const Rect& t) {
    return STR_RIGHT_LTE(p, t) && WITHIN_Y(p, t);
}

inline bool CON_TOP(const Rect& p, const Rect& t) {
    return STR_TOP_LTE(p, t) && WITHIN_X(p, t);
}

inline bool CON_BOTTOM(const Rect& p, const Rect& t) {
    return STR_BOTTOM_LTE(p, t) && WITHIN_X(p, t);
}

#endif /* CollisionHelper_h */
