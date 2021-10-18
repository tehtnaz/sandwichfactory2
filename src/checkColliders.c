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

CollisionInfo checkAllColliders(Rectangle self, bool checkObjects, int colliderNum, int ladderNum, int crateNum, int leverNum, int doorNum, BoxCollider2D Col[15], PhysicsObject crate[2]){
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

    int colsToCheck = colliderNum + leverNum + doorNum;
    if(checkObjects && crateNum > 0 ) colsToCheck += crateNum;

    for(int i = 0; i < colsToCheck; i++){
        if(i > colliderNum - 1 + leverNum + doorNum){
            f_box.x = crate[i - colliderNum].position.x;
            f_box.y = crate[i - colliderNum].position.y;
            f_box.width = crate[i - colliderNum].sizeX;
            f_box.height = crate[i - colliderNum].sizeY;
            result = f_checkCollider(f_box, self, crate[i - colliderNum].trigger, crate[i - colliderNum].enabled);
            printf("checking obj: %d\n", i - colliderNum);
        }else{
            box[0] = Col[i].x;
            box[1] = Col[i].y;
            box[2] = Col[i].sizeX;
            box[3] = Col[i].sizeY;
            result = checkCollider(box, self, Col[i].trigger, Col[i].enabled, Col[i].ladder);
        }
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
                collision.inTrigger = true;
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