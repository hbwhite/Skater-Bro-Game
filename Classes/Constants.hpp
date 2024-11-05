//
//  Constants.hpp
//  chocolate
//
//  Created by Harrison White on 1/24/18.
//
//

#ifndef Constants_h
#define Constants_h

// jump dynamics

// circle player geometry
static const float  PLAYER_VELOCITY     = 12;
static const float  JUMP_VELOCITY       = 40; /* Impulse of 22 when using PhysicsEditor */ /* 45 for single impulse, 30 for sustained jump system */
static const float  GRAVITY             = -120;

// square player geometry
/*
static const float  PLAYER_DENSITY      = 1.0f;
static const float  JUMP_IMPULSE        = 45;
static const float  MOVE_IMPULSE        = 10;
static const float  GRAVITY             = -120;
*/


/**
 LEGACY PARAMETERS THAT WORKED WELL:
 
 static const int    PLAYER_SPEED_PPS    = 300;      // 300/32 = 9.375 m/s
 static const float  ACCELERATION_Y      = -1800;
 static const float  JUMP_VELOCITY       = 1300;
 
 // max distance player can travel from camera focus
 // positive -> camera will lag behind player
 // negative -> camera will track ahead of player
 static const int    PLAYER_X_OFFSET     = -130;
 static const int    PLAYER_Y_OFFSET     = -50;
 */

#endif /* Constants_h */
