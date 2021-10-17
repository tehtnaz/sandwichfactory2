#include "levelStructs.h"
#pragma once
/*
*  0 = x Position
*  1 = y Position
*  2 = size X
*  3 = size Y
*/

bool up(int box[4], float self[4]);
bool left(int box[4], float self[4]);
bool right(int box[4], float self[4]);
bool down(int box[4], float self[4]);


bool betweenX(int box[4], float self[4]);
bool betweenY(int box[4], float self[4]);

bool objectBetweenX(int box[4], float self[4]);
bool objectBetweenY(int box[4], float self[4]);

bool botLeftEdgeCheck(int box[4], float self[4]);
bool botRightEdgeCheck(int box[4], float self[4]);
bool topLeftEdgeCheck(int box[4], float self[4]);
bool topRightEdgeCheck(int box[4], float self[4]);

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
int checkCollider(int box[4], float self[4], bool trigger, bool enabled, bool ladder);

typedef struct CollisionInfo{
    bool left;
    bool right;
    bool up;
    bool down;

    int colsTouched[14];
    //bool sidesTouched[4];
    int floor;

    bool inTrigger;
    int triggerID;

    bool inLadder;
}CollisionInfo;

typedef struct BoxColInfo{
    bool topLeft;
    bool topRight;
    bool bottomLeft;
    bool bottomRight;

    int colsTouched[14];
    //bool sidesTouched[4];
    int floor;

    bool inTrigger;
    int triggerID;

    bool inLadder;
}BoxColInfo;

CollisionInfo checkAllColliders(float self[4], bool checkObjects, int colliderNum, int ladderNum, int crateNum, int leverNum, int doorNum, BoxCollider2D Col[15], PhysicsObject crate[2]);

CollisionInfo checkObjects(CollisionInfo collision, float self[4], int selfObjID, int crateNum, PhysicsObject crate[2]);