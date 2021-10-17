#pragma once
#include <stdbool.h>
/*
*  0 = x Position
*  1 = y Position
*  2 = size X
*  3 = size Y
*/

bool f_up(float box[4], float self[4]);
bool f_left(float box[4], float self[4]);
bool f_right(float box[4], float self[4]);
bool f_down(float box[4], float self[4]);


bool f_betweenX(float box[4], float self[4]);
bool f_betweenY(float box[4], float self[4]);

bool f_objectBetweenX(float box[4], float self[4]);
bool f_objectBetweenY(float box[4], float self[4]);

bool f_botLeftEdgeCheck(float box[4], float self[4]);
bool f_botRightEdgeCheck(float box[4], float self[4]);
bool f_topLeftEdgeCheck(float box[4], float self[4]);
bool f_topRightEdgeCheck(float box[4], float self[4]);

int f_checkCollider(float box[4], float self[4], bool trigger, bool enabled);

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
