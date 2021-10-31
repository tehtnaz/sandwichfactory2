#ifndef VELOCITY_H

#include "levelStructs.h"
#include "checkColliders.h"
#include "checkColliders_Float.h"

#define VELOCITY_H

bool isTouchingPlayerGrabZone(PhysicsObject input, Vector2 playerPosition, Vector2 playerSize, int resMultiplier);


//properties line 1 = player collision / velocity change arguments
//           line 2 = general environment / visual properties 
//           line 3 = check object against other objects (collision detection)
PhysicsObject updateObject(
        PhysicsObject input, Vector2 playerPosition, Vector2 playerPosition2, Vector2 playerSize, Vector2 playerSize2, Vector2 playerVelocity, Vector2 playerVelocity2, 
        float friction, int screenFPS, float gravity, int crateID, bool flipX, int resMultiplier,
        int colliderNum, int ladderNum, int crateNum, int leverNum, int doorNum, BoxCollider2D Col[15], PhysicsObject crate[2]
    );


#endif