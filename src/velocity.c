#include "levelStructs.h"
#include "checkColliders.h"
#include "checkColliders_Float.h"
#include "velocity.h"

/*
* 0 = None
* 1 = Left
* 2 = Right
* 3 = Up
* 4 = Down
* 5 = Trigger
*/

bool isTouchingPlayer(PhysicsObject input, Vector2 playerPosition, Vector2 playerSize, int resMultiplier){
    float self[4];
    self[0] = input.position.x;
    self[1] = input.position.y;
    self[2] = input.sizeX;
    self[3] = input.sizeY;
    float box[4];
    box[0] = playerPosition.x - (resMultiplier * 2);
    box[1] = playerPosition.y + resMultiplier;
    box[2] = playerSize.x + (resMultiplier * 4);
    box[3] = playerSize.y - (resMultiplier * 2);
    int result = f_checkCollider(box, self, false, true);
    if(result == 0){
        return false;
    }else{
        return true;
    }

}

PhysicsObject updateObject(
        PhysicsObject input, Vector2 playerPosition, Vector2 playerPosition2, Vector2 playerSize, Vector2 playerVelocity, Vector2 playerVelocity2, float friction, int screenFPS, float gravity, int crateID, bool flipX, int resMultiplier,
        int colliderNum, int ladderNum, int crateNum, int leverNum, int doorNum, BoxCollider2D Col[15], PhysicsObject crate[2]
    ){
    
    PhysicsObject temp = input;
    float fpsMultiplier = (1.0f/(float)screenFPS);

    //Friction
    temp.velocity.x -= (temp.velocity.x - (temp.velocity.x * friction)) * 60 / screenFPS;

    //Check against level colliders
    float self[4];
    self[0] = input.position.x;
    self[1] = input.position.y;
    self[2] = input.sizeX;
    self[3] = input.sizeY;
    CollisionInfo objCollision = checkAllColliders(self, false, colliderNum, ladderNum, crateNum, leverNum, doorNum, Col, crate);
    objCollision = checkObjects(objCollision, self, crateID, crateNum, crate);

    //Check against given player
    float box[4];
    box[0] = playerPosition.x;
    box[1] = playerPosition.y;
    box[2] = playerSize.x;
    box[3] = playerSize.y;
    int result = f_checkCollider(box, self, false, true);
    switch(result){
        case 1:
            objCollision.left = true;
            temp.velocity.x = playerVelocity.x;
            break;
        case 2:
            objCollision.right = true;
            temp.velocity.x = playerVelocity.x;
            break;
        case 3:
            objCollision.up = true;
            break;
        case 4:
            objCollision.down = true;
            break;
    }
    if(result == 0){
        temp.isTouchingPlayer = false;
    }else{
        temp.isTouchingPlayer = true;
    }

    //Player 2
    box[0] = playerPosition2.x;
    box[1] = playerPosition2.y;
    box[2] = playerSize.x;
    box[3] = playerSize.y;
    result = f_checkCollider(box, self, false, true);
    switch(result){
        case 1:
            objCollision.left = true;
            temp.velocity.x = playerVelocity2.x;
            break;
        case 2:
            objCollision.right = true;
            temp.velocity.x = playerVelocity2.x;
            break;
        case 3:
            objCollision.up = true;
            break;
        case 4:
            objCollision.down = true;
            break;
    }
    if(result == 0){
        temp.isTouchingPlayer = false;
    }else{
        temp.isTouchingPlayer = true;
    }

    if(IsKeyDown(KEY_E) && isTouchingPlayer(temp, playerPosition, playerSize, resMultiplier)){
        temp.velocity.x = playerVelocity.x;
        temp.velocity.y = -playerVelocity.y;
    }

    if(IsKeyDown(KEY_RIGHT_CONTROL) && isTouchingPlayer(temp, playerPosition2, playerSize, resMultiplier)){
        temp.velocity.x = playerVelocity2.x;
        temp.velocity.y = -playerVelocity2.y;
    }

    //Change velocity based on collision
    if(!objCollision.down){
        temp.velocity.y -= gravity / (screenFPS / 60.00f);
    }

    if((objCollision.down && temp.velocity.y > 0) || (objCollision.up && temp.velocity.y < 0)){
        
        temp.velocity.y = 0;
        //if (objCollision.down && temp.velocity.y > 0) temp.position.y = Col[objCollision.floor].y - temp.sizeY + 0.1f;
    }
    if((objCollision.left && temp.velocity.x < 0) || (objCollision.right && temp.velocity.x > 0)){
        temp.velocity.x = 0;
    }

    //Update Velocity
    temp.position.x += temp.velocity.x * fpsMultiplier;
    temp.position.y += temp.velocity.y * fpsMultiplier;

    //printf("x = %f\n", temp.velocity.x);
    //printf("y = %f\n", temp.velocity.y);
    //printf("touch = %d\n", temp.isTouchingPlayer);
    return temp;
}