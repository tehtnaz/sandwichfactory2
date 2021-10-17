#ifndef VELOCITY_H

#include "levelStructs.h"
#include "checkColliders.h"
#include "checkColliders_Float.h"

#define VELOCITY_H

bool isTouchingPlayer(PhysicsObject input, Vector2 playerPosition, Vector2 playerSize, int resMultiplier);
PhysicsObject updateObject(
        PhysicsObject input, Vector2 playerPosition, Vector2 playerPosition2, Vector2 playerSize, Vector2 playerVelocity, Vector2 playerVelocity2, float friction, int screenFPS, float gravity, int crateID, bool flipX, int resMultiplier,
        int colliderNum, int ladderNum, int crateNum, int leverNum, int doorNum, BoxCollider2D Col[15], PhysicsObject crate[2]
    );


#endif