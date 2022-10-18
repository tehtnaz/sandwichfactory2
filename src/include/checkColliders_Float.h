#ifndef CHECKCOLLIDERS_FLOAT_H
    #define CHECKCOLLIDERS_FLOAT_H
    #include <stdbool.h>
    #include "raylib.h"

    //checkColliders_Float.h version 2.1
    //changelog
        //version 2: changed float arrays into Rectangle type
        //version 2.1: added comments, removed pragma, used define


    #ifndef RAYLIB_H
        #ifndef RECTANGLE_DEFINED
            #define RECTANGLE_DEFINED
            typedef struct Rectangle {
                float x;
                float y;
                float width;
                float height;
            } Rectangle;
        #endif
    #endif

    //is rectangle in (direction) compared to other and touching?
    bool f_up(Rectangle box, Rectangle self);
    bool f_left(Rectangle box, Rectangle self);
    bool f_right(Rectangle box, Rectangle self);
    bool f_down(Rectangle box, Rectangle self);

    //is rectangle between width/height of other rectangle?
    bool f_betweenX(Rectangle box, Rectangle self);
    bool f_betweenY(Rectangle box, Rectangle self);

    bool f_objectBetweenX(Rectangle box, Rectangle self);
    bool f_objectBetweenY(Rectangle box, Rectangle self);

    bool f_botLeftEdgeCheck(Rectangle box, Rectangle self);
    bool f_botRightEdgeCheck(Rectangle box, Rectangle self);
    bool f_topLeftEdgeCheck(Rectangle box, Rectangle self);
    bool f_topRightEdgeCheck(Rectangle box, Rectangle self);

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
    int f_checkCollider(Rectangle box, Rectangle self, bool trigger, bool enabled);
#endif