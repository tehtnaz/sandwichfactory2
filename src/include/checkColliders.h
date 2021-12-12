#include "levelStructs.h"
#include "raylib.h"
#pragma once
/*
*  0 = x Position
*  1 = y Position
*  2 = size X
*  3 = size Y
*/

//checkColliders version 2
//changelog
//version 2: changed float arrays into Rectangle type

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

bool up(int box[4], Rectangle self);
bool left(int box[4], Rectangle self);
bool right(int box[4], Rectangle self);
bool down(int box[4], Rectangle self);


bool betweenX(int box[4], Rectangle self);
bool betweenY(int box[4], Rectangle self);

bool objectBetweenX(int box[4], Rectangle self);
bool objectBetweenY(int box[4], Rectangle self);

bool botLeftEdgeCheck(int box[4], Rectangle self);
bool botRightEdgeCheck(int box[4], Rectangle self);
bool topLeftEdgeCheck(int box[4], Rectangle self);
bool topRightEdgeCheck(int box[4], Rectangle self);

/*
* Return value = direction of collision
* Return value definitions:
* 0 = None
* 1 = Left
* 2 = Right
* 3 = Up
* 4 = Down
* 5 = Trigger
* 6 = ladder

*  0 = x Position
*  1 = y Position
*  2 = size X
*  3 = size Y
*/
int checkCollider(int box[4], Rectangle self, bool trigger, bool enabled, bool ladder);

typedef struct CollisionInfo{
    bool left;
    bool right;
    bool up;
    bool down;

    int colsTouched[64];
    //bool sidesTouched[4];
    int floor;

    int inTrigger;
    int triggerObjID;

    bool inLadder;
}CollisionInfo;

typedef struct BoxColInfo{
    bool topLeft;
    bool topRight;
    bool bottomLeft;
    bool bottomRight;

    int colsTouched[64];
    //bool sidesTouched[4];
    int floor;

    int inTrigger;
    int triggerID;

    bool inLadder;
}BoxColInfo;

CollisionInfo checkAllColliders(Rectangle self, bool checkObjects, int colliderNum, int ladderNum, int crateNum, int leverNum, int doorNum, int portalNum, BoxCollider2D Col[64], PhysicsObject crate[8], BoxCollider2D ladderCol[16]);

CollisionInfo checkObjects(CollisionInfo collision, Rectangle self, int selfObjID, int crateNum, PhysicsObject crate[8]);