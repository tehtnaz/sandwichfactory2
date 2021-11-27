#include "levelStructs.h"
#include "checkColliders.h"
#include "checkColliders_Float.h"
#include "velocity.h"
#include "recpp.h"

/*
* 0 = None
* 1 = Left
* 2 = Right
* 3 = Up
* 4 = Down
* 5 = Trigger
*/

bool isTouchingPlayerGrabZone(PhysicsObject input, Vector2 playerPosition, Vector2 playerSize, int resMultiplier){
    Rectangle self;
    self.x = input.position.x;
    self.y = input.position.y;
    self.width = input.sizeX;
    self.height = input.sizeY;
    Rectangle box;
    box.x = playerPosition.x - (resMultiplier * 2);
    box.y = playerPosition.y + resMultiplier;
    box.width = playerSize.x + (resMultiplier * 4);
    box.height = playerSize.y - (resMultiplier * 2);
    int result = f_checkCollider(box, self, false, true);
    if(result == 0){
        return false;
    }else{
        return true;
    }

}



PhysicsObject updateObject(
        PhysicsObject input, Vector2 playerPosition, Vector2 playerPosition2, Vector2 playerSize, Vector2 playerSize2, Vector2 playerVelocity, Vector2 playerVelocity2, 
        float friction, float bigFriction, int screenFPS, float gravity, int crateID, bool flipX, int resMultiplier,
        int colliderNum, int ladderNum, int crateNum, int leverNum, int doorNum, int portalNum,
        BoxCollider2D Col[15], PhysicsObject crate[2], BoxCollider2D ladderCol[2]
    ){
    
    //make sure no properties are erased when returned
    PhysicsObject temp = input;

    //Check against level colliders
    Rectangle self = combineVec2(input.position, (Vector2){input.sizeX, input.sizeY});
    CollisionInfo objCollision = checkAllColliders(self, false, colliderNum, ladderNum, crateNum, leverNum, doorNum, portalNum, Col, crate, ladderCol);
    objCollision = checkObjects(objCollision, self, crateID, crateNum, crate);

    //Check against given player
    Rectangle box = combineVec2(playerPosition, playerSize);
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
    box = combineVec2(playerPosition2, playerSize2);
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


    //Friction
    if(objCollision.down) temp.velocity.x -= (temp.velocity.x - (temp.velocity.x * bigFriction)) * 60 / screenFPS;
    else temp.velocity.x -= (temp.velocity.x - (temp.velocity.x * friction)) * 60 / screenFPS;



    //Is being grabbed by player?
    if(IsKeyDown(KEY_E) && isTouchingPlayerGrabZone(temp, playerPosition, playerSize, resMultiplier)){
        temp.velocity.x = playerVelocity.x;
        temp.velocity.y = -playerVelocity.y;
    }

    //How about player 2?
    if(IsKeyDown(KEY_RIGHT_CONTROL) && isTouchingPlayerGrabZone(temp, playerPosition2, playerSize2, resMultiplier)){
        temp.velocity.x = playerVelocity2.x;
        temp.velocity.y = -playerVelocity2.y;
    }



    //Change velocity based on collision
    if(!objCollision.down){
        temp.velocity.y -= gravity * (60.00f / screenFPS);
    }

    if((objCollision.down && temp.velocity.y > 0) || (objCollision.up && temp.velocity.y < 0)){
        
        temp.velocity.y = 0;
        //if (objCollision.down && temp.velocity.y > 0) temp.position.y = Col[objCollision.floor].y - temp.sizeY + 0.1f;
    }
    if((objCollision.left && temp.velocity.x < 0) || (objCollision.right && temp.velocity.x > 0)){
        temp.velocity.x = 0;
    }

    //Update Velocity
    temp.position.x += temp.velocity.x / screenFPS;
    temp.position.y += temp.velocity.y / screenFPS;
    return temp;
}