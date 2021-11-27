#include "checkColliders_Float.h"
#include <stdbool.h>
#include <stdio.h>
#include "levelStructs.h"
#include "checkColliders.h"
#include "raylib.h"
/*
*  0 = x Position
*  1 = y Position
*  2 = size X
*  3 = size Y
*/

//checkColliders version 2
//changelog
//version 2: changed float arrays into Rectangle type

bool up(int box[4], Rectangle self){
    if((self.y < box[1] + box[3] && self.y > box[1]) || self.y == box[1]){
        return true;
    }else{
        return false;
    }
}
bool left(int box[4], Rectangle self){
    if(self.x < box[0] + box[2] && self.x > box[0]){
        return true;
    }else{
        return false;
    }
}
bool right(int box[4], Rectangle self){
    if(self.x + self.width > box[0] && self.x + self.width < box[0] + box[2]){
        return true;
    }else{
        return false;
    }
}
bool down(int box[4], Rectangle self){
    if((self.y + self.height > box[1] && self.y + self.height < box[1] + box[3]) || self.y + self.height == box[1] + box[3]){
        return true;
    }else{
        return false;
    }
}


bool betweenX(int box[4], Rectangle self){
    if(self.x > box[0] && self.x + self.width < box[0] + box[2]){
        return true;
    }else{
        return false;
    }
}
bool betweenY(int box[4], Rectangle self){
    if(self.y > box[1] && self.y + self.height < box[1] + box[3]){
        return true;
    }else{
        return false;
    }
}

bool objectBetweenX(int box[4], Rectangle self){
    if(box[0] > self.x && box[0] + box[2] < self.x + self.width){
        return true;
    }else{
        return false;
    }
}
bool objectBetweenY(int box[4], Rectangle self){
    if(box[1] > self.y && box[1] + box[3] < self.y + self.height){
        return true;
    }else{
        return false;
    }
}

bool botLeftEdgeCheck(int box[4], Rectangle self){
    int distanceY = self.y + self.height - box[1];
    int distanceX = box[0] + box[2] - self.x;
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}
bool botRightEdgeCheck(int box[4], Rectangle self){
    int distanceY = self.y + self.height - box[1];
    int distanceX = self.x + self.width - box[0];
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}
bool topLeftEdgeCheck(int box[4], Rectangle self){
    int distanceY = box[1] + box[3] - self.y;
    int distanceX = box[0] + box[2] - self.x;
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}
bool topRightEdgeCheck(int box[4], Rectangle self){
    int distanceY = box[1] + box[3] - self.y;
    int distanceX = self.x + self.width - box[0];
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}

/*
* Return value = direction of collision
* Return value definitions:
* 0 = None
* 1 = Left
* 2 = Right
* 3 = Up
* 4 = Down
*/

/*
*  0 = x Position
*  1 = y Position
*  2 = size X
*  3 = size Y
*/
int checkCollider(int box[4], Rectangle self, bool trigger, bool enabled, bool ladder){
    if(enabled == false){
        return 0;
    }

    if(betweenX(box, self) || objectBetweenX(box, self)){
        //printf("X %d    ", id);
        if(up(box, self)){
            if(trigger) {
                return 5;
            }
            if(ladder) {
                return 6;
            }
            return 3;
        }else if(down(box, self)){
            if(trigger) {
                return 5;
            }
            if(ladder) {
                return 6;
            }
            return 4;
        }
    }else if (betweenY(box, self) || objectBetweenY(box, self)){
        //printf("Y %d    ", id);
        if(left(box, self)){
            if(trigger) {
                return 5;
            }
            if(ladder) {
                return 6;
            }
            return 1;
        }else if(right(box, self)){
            if(trigger) {
                return 5;
            }
            if(ladder) {
                return 6;
            }
            return 2;
        }
    }else{
        if(left(box, self) && (up(box, self) || down(box, self))){
            if(trigger) {
                return 5;
            }
            if(ladder) {
                return 6;
            }

            if(up(box, self)){
                //True = horizontal
                //False = vertical

                if(topLeftEdgeCheck(box, self)){
                    return 1;
                }else{
                    return 3;
                }
            }else{
                if(botLeftEdgeCheck(box, self)){
                    return 1;
                }else{
                    return 4;
                }
            }
        }
        if(right(box, self) && (up(box, self) || down(box, self))){
            if(trigger) {
                return 5;
            }
            if(ladder) {
                return 6;
            }

            if(up(box, self)){
                //True = horizontal
                //False = vertical
                if(topRightEdgeCheck(box, self)){
                    return 2;
                }else{
                    return 3;
                }
            }else{
                if(botRightEdgeCheck(box, self)){
                    return 2;
                }else{
                    return 4;
                }
            }
        }
    }
    return 0;
}

/*
* 0 = None
* 1 = Left
* 2 = Right
* 3 = Up
* 4 = Down
* 5 = Trigger
* 6 = ladder
*/

//outputs which sides of a rectangle have contacted any other collider
CollisionInfo checkAllColliders(Rectangle self, bool checkObjects, int colliderNum, int ladderNum, int crateNum, int leverNum, int doorNum, int portalNum, BoxCollider2D Col[25], PhysicsObject crate[2], BoxCollider2D ladderCol[5]){
    CollisionInfo collision;
    int box[4];
    Rectangle f_box;
    int result;

    collision.left = false;
    collision.right = false;
    collision.up = false;
    collision.down = false;
    collision.inTrigger = false;
    collision.inLadder = false;

    int colsToCheck = colliderNum + leverNum + doorNum + ladderNum + portalNum;
    if(checkObjects && crateNum > 0 ) colsToCheck += crateNum;

    for(int i = 0; i < colsToCheck; i++){
        if(i > colliderNum - 1 + leverNum + doorNum + ladderNum + portalNum){
            f_box.x = crate[i - colliderNum - leverNum - doorNum - portalNum].position.x;
            f_box.y = crate[i - colliderNum - leverNum - doorNum - portalNum].position.y;
            f_box.width = crate[i - colliderNum - leverNum - doorNum - portalNum].sizeX;
            f_box.height = crate[i - colliderNum - leverNum - doorNum - portalNum].sizeY;
            result = f_checkCollider(f_box, self, crate[i - colliderNum - leverNum - doorNum - portalNum].trigger, crate[i - colliderNum - leverNum - doorNum - portalNum].enabled);
            //printf("checking obj: %d\n", i - colliderNum - leverNum - doorNum);
        }else{
            if(i > colliderNum - 1 + leverNum + doorNum + portalNum){
                box[0] = ladderCol[i - colliderNum - leverNum - doorNum - portalNum].x;
                box[1] = ladderCol[i - colliderNum - leverNum - doorNum - portalNum].y;
                box[2] = ladderCol[i - colliderNum - leverNum - doorNum - portalNum].sizeX;
                box[3] = ladderCol[i - colliderNum - leverNum - doorNum - portalNum].sizeY;
                result = checkCollider(box, self, ladderCol[i - colliderNum - leverNum - doorNum - portalNum].trigger, ladderCol[i - colliderNum - leverNum - doorNum - portalNum].enabled, true);
                //result = 6;
                //printf("checking ladder: %d     ", i - colliderNum - leverNum - doorNum);
            }else{
                box[0] = Col[i].x;
                box[1] = Col[i].y;
                box[2] = Col[i].sizeX;
                box[3] = Col[i].sizeY;
                result = checkCollider(box, self, Col[i].trigger, Col[i].enabled, Col[i].ladder);
                //printf("checking col: %d      ", i);
            }
        }
        //printf("result: %d\n", result);
        collision.colsTouched[i] = result;
        switch(result){
            case 1:
                collision.left = true;
                break;
            case 2:
                collision.right = true;
                break;
            case 3:
                collision.up = true;
                break;
            case 4:
                collision.down = true;
                collision.floor = i;
                break;
            case 5:
                if(i > colliderNum - 1 + leverNum + doorNum + ladderNum + portalNum){
                    collision.inTrigger = crate[i - colliderNum - leverNum - doorNum - portalNum].trigger;
                }else{
                    if(i > colliderNum - 1 + leverNum + doorNum + portalNum){
                        collision.inTrigger = ladderCol[i - colliderNum - leverNum - doorNum - portalNum].trigger;
                    }else{
                        collision.inTrigger = Col[i].trigger;
                    }
                }
                collision.triggerObjID = i;
                break;
            case 6:
                collision.inLadder = true;
                break;
        }
    }
    return collision;
}

CollisionInfo checkObjects(CollisionInfo collision, Rectangle self, int selfObjID, int crateNum, PhysicsObject crate[2]){

    //This will cause the use of more memory
    Rectangle f_box;
    int result;

    int colsToCheck = crateNum;
    //printf("%d\n", colsToCheck);

    for(int i = 0; i < colsToCheck; i++){

        f_box.x = crate[i].position.x;
        f_box.y = crate[i].position.y;
        f_box.width = crate[i].sizeX;
        f_box.height = crate[i].sizeY;

        if(i == selfObjID){
            result = 0;
        }else{
            result = f_checkCollider(f_box, self, crate[i].trigger, crate[i].enabled);
        }

        if(result != 0){
            collision.colsTouched[i] = result;
        }
        if(result == 1){
            collision.left = true;
            //crate[i].velocity.x = crate[selfObjID].velocity.x;
        }
        if(result == 2){
            collision.right = true;
            //crate[i].velocity.x = crate[selfObjID].velocity.x;
        }
        if(result == 3){
            collision.up = true;
        }
        if(result == 4){
            collision.down = true;
            collision.floor = i;
        }
        if(result == 5){
            collision.inTrigger = true;
        }
    }
    return collision;
}