#include <stdbool.h>
#include "checkColliders_Float.h"
#include "raylib.h"
/*
*  0 = x Position
*  1 = y Position
*  2 = size X
*  3 = size Y
*/

//checkColliders_Float.h version 2
//changelog
//version 2: changed float arrays into Rectangle type

bool f_up(Rectangle box, Rectangle self){
    if((self.y < box.y + box.height && self.y > box.y) || self.y == box.y){
        return true;
    }else{
        return false;
    }
}
bool f_left(Rectangle box, Rectangle self){
    if(self.x < box.x + box.width && self.x > box.x){
        return true;
    }else{
        return false;
    }
}
bool f_right(Rectangle box, Rectangle self){
    if(self.x + self.width > box.x && self.x + self.width < box.x + box.width){
        return true;
    }else{
        return false;
    }
}
bool f_down(Rectangle box, Rectangle self){
    if((self.y + self.height > box.y && self.y + self.height < box.y + box.height) || self.y + self.height == box.y + box.height){
        return true;
    }else{
        return false;
    }
}


bool f_betweenX(Rectangle box, Rectangle self){
    if(self.x > box.x && self.x + self.width < box.x + box.width){
        return true;
    }else{
        return false;
    }
}
bool f_betweenY(Rectangle box, Rectangle self){
    if(self.y > box.y && self.y + self.height < box.y + box.height){
        return true;
    }else{
        return false;
    }
}

bool f_objectBetweenX(Rectangle box, Rectangle self){
    if(box.x > self.x && box.x + box.width < self.x + self.width){
        return true;
    }else{
        return false;
    }
}
bool f_objectBetweenY(Rectangle box, Rectangle self){
    if(box.y > self.y && box.y + box.height < self.y + self.height){
        return true;
    }else{
        return false;
    }
}

bool f_botLeftEdgeCheck(Rectangle box, Rectangle self){
    float distanceY = self.y + self.height - box.y;
    float distanceX = box.x + box.width - self.x;
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}
bool f_botRightEdgeCheck(Rectangle box, Rectangle self){
    float distanceY = self.y + self.height - box.y;
    float distanceX = self.x + self.width - box.x;
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}
bool f_topLeftEdgeCheck(Rectangle box, Rectangle self){
    float distanceY = box.y + box.height - self.y;
    float distanceX = box.x + box.width - self.x;
    if(distanceX < distanceY){
        //horizontal collision
        return true;
    }else{
        //vertical collision
        return false;
    }
}
bool f_topRightEdgeCheck(Rectangle box, Rectangle self){
    float distanceY = box.y + box.height - self.y;
    float distanceX = self.x + self.width - box.x;
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
int f_checkCollider(Rectangle box, Rectangle self, bool trigger, bool enabled){
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
