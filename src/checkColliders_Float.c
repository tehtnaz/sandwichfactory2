#include <stdbool.h>
#include "checkColliders_Float.h"
/*
*  0 = x Position
*  1 = y Position
*  2 = size X
*  3 = size Y
*/

bool f_up(float box[4], float self[4]){
    if((self[1] < box[1] + box[3] && self[1] > box[1]) || self[1] == box[1]){
        return true;
    }else{
        return false;
    }
}
bool f_left(float box[4], float self[4]){
    if(self[0] < box[0] + box[2] && self[0] > box[0]){
        return true;
    }else{
        return false;
    }
}
bool f_right(float box[4], float self[4]){
    if(self[0] + self[2] > box[0] && self[0] + self[2] < box[0] + box[2]){
        return true;
    }else{
        return false;
    }
}
bool f_down(float box[4], float self[4]){
    if((self[1] + self[3] > box[1] && self[1] + self[3] < box[1] + box[3]) || self[1] + self[3] == box[1] + box[3]){
        return true;
    }else{
        return false;
    }
}


bool f_betweenX(float box[4], float self[4]){
    if(self[0] > box[0] && self[0] + self[2] < box[0] + box[2]){
        return true;
    }else{
        return false;
    }
}
bool f_betweenY(float box[4], float self[4]){
    if(self[1] > box[1] && self[1] + self[3] < box[1] + box[3]){
        return true;
    }else{
        return false;
    }
}

bool f_objectBetweenX(float box[4], float self[4]){
    if(box[0] > self[0] && box[0] + box[2] < self[0] + self[2]){
        return true;
    }else{
        return false;
    }
}
bool f_objectBetweenY(float box[4], float self[4]){
    if(box[1] > self[1] && box[1] + box[3] < self[1] + self[3]){
        return true;
    }else{
        return false;
    }
}

bool f_botLeftEdgeCheck(float box[4], float self[4]){
    float distanceY = self[1] + self[3] - box[1];
    float distanceX = box[0] + box[2] - self[0];
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}
bool f_botRightEdgeCheck(float box[4], float self[4]){
    float distanceY = self[1] + self[3] - box[1];
    float distanceX = self[0] + self[2] - box[0];
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}
bool f_topLeftEdgeCheck(float box[4], float self[4]){
    float distanceY = box[1] + box[3] - self[1];
    float distanceX = box[0] + box[2] - self[0];
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}
bool f_topRightEdgeCheck(float box[4], float self[4]){
    float distanceY = box[1] + box[3] - self[1];
    float distanceX = self[0] + self[2] - box[0];
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}

/*
*  0 = x Position
*  1 = y Position
*  2 = size X
*  3 = size Y
*/

/*
* Return value = direction of collision
* Return value definitions:
* 0 = None
* 1 = Left
* 2 = Right
* 3 = Up
* 4 = Down
* 5 = trigger
*/
int f_checkCollider(float box[4], float self[4], bool trigger, bool enabled){
    if(enabled == false){
        return 0;
    }

    if(f_betweenX(box, self) || f_objectBetweenX(box, self)){
        //prfloatf("X %d    ", id);
        if(f_up(box, self)){
            if(trigger) return 5;
            return 3;
        }else if(f_down(box, self)){
            if(trigger) return 5;
            return 4;
        }
    }else if (f_betweenY(box, self) || f_objectBetweenY(box, self)){
        //prfloatf("Y %d    ", id);
        if(f_left(box, self)){
            if(trigger) return 5;
            return 1;
        }else if(f_right(box, self)){
            if(trigger) return 5;
            return 2;
        }
    }else{
        if(f_left(box, self) && (f_up(box, self) || f_down(box, self))){
            if(trigger) return 5;

            if(f_up(box, self)){
                //True = horizontal
                //False = vertical

                if(f_topLeftEdgeCheck(box, self)){
                    return 1;
                }else{
                    return 3;
                }
            }else{
                if(f_botLeftEdgeCheck(box, self)){
                    return 1;
                }else{
                    return 4;
                }
            }
        }
        if(f_right(box, self) && (f_up(box, self) || f_down(box, self))){
            if(trigger) return 5;

            if(f_up(box, self)){
                //True = horizontal
                //False = vertical
                if(f_topRightEdgeCheck(box, self)){
                    return 2;
                }else{
                    return 3;
                }
            }else{
                if(f_botRightEdgeCheck(box, self)){
                    return 2;
                }else{
                    return 4;
                }
            }
        }
    }
    return 0;
}
