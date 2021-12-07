#ifndef VELOCITY_H

#include "levelStructs.h"
#include "checkColliders.h"
#include "checkColliders_Float.h"

#define VELOCITY_H

bool isTouchingPlayerGrabZone(PhysicsObject input, Vector2 playerPosition, Vector2 playerSize, int resMultiplier);


//properties line 1 = player collision / velocity change arguments
//           line 2 = general environment / visual properties 
//           line 3/4 = check object against other objects (collision detection)
PhysicsObject updateObject(
        PhysicsObject input, Vector2 playerPosition, Vector2 playerPosition2, Vector2 playerSize, Vector2 playerSize2, Vector2 playerVelocity, Vector2 playerVelocity2, 
        float friction, float bigFriction, int screenFPS, float gravity, int crateID, bool flipX, int resMultiplier,
        int colliderNum, int ladderNum, int crateNum, int leverNum, int doorNum, int portalNum, 
        BoxCollider2D Col[64], PhysicsObject crate[8], BoxCollider2D ladderCol[16]
    );


#endif