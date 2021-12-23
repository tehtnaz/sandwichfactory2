#ifndef VECTOR2PP_H

#include <stdio.h>
#include "raylib.h"

//vector2pp Version 2

#define VECTOR2PP_H
#define VEC2ZERO (Vector2){0,0}

Vector2 addVec2(Vector2 input, Vector2 input2);
Vector2 subVec2(Vector2 input, Vector2 input2);
Vector2 negVec2(Vector2 input);
Vector2 addNewVec2(Vector2 input, float inX, float inY);
Vector2 resizeVec2(Vector2 input, float multiplier);

//deprecated because (Vector2){0,0} is a thing
//Vector2 newVec2(float inX, float inY);

float distanceToObject(Vector2 source, float sourceRadius, Vector2 dest, float destRadius, bool returnSubRadius);

//Linear interpolation (Lerp)
//lerpAmount changes the input a certain percentage of the distance of the input to the destination
Vector2 lerpVec2(Vector2 input, float destX, float destY, float lerpAmount);


#endif