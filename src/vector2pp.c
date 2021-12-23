#include <stdio.h>
#include "raylib.h"
#include "vector2pp.h"
#include <math.h>

//vector2pp Version 2

Vector2 addVec2(Vector2 input, Vector2 input2){
    return (Vector2){input.x + input2.x, input.y + input2.y};
}

Vector2 subVec2(Vector2 input, Vector2 input2){
    return (Vector2){input.x - input2.x, input.y - input2.y};
}

Vector2 negVec2(Vector2 input){
    return (Vector2){-input.x, -input.y};
}

Vector2 addNewVec2(Vector2 input, float inX, float inY){
    Vector2 temp = input;
    temp.x += inX;
    temp.y += inY;
    return temp;
}

Vector2 resizeVec2(Vector2 input, float multiplier){
    return (Vector2){input.x * multiplier, input.y * multiplier};
}

//deprecated because (Vector2){0,0} is a thing
/*Vector2 newVec2(float inX, float inY){
    Vector2 temp;
    temp.x = inX;
    temp.y = inY;
    return temp;
}*/


float distanceToObject(Vector2 source, float sourceRadius, Vector2 dest, float destRadius, bool returnSubRadius){
    double a = source.x - dest.x;
    double b = source.y - dest.y;
    printf("!! a = %f; !! b = %f", a, b);
    float c = sqrt((a*a) + (b*b));
    if(returnSubRadius){
        return c - sourceRadius - destRadius;
    }else{
        return c;
    }
}

//Linear interpolation (Lerp)
//lerpAmount changes the input a certain percentage of the distance of the input to the destination

Vector2 lerpVec2(Vector2 input, float destX, float destY, float lerpAmount){
    Vector2 temp = input;
    if(lerpAmount == 0){
        printf("WARNING: lerpAmount has been set to zero!\n");
        return temp;
    }
    temp.x += (destX - temp.x) * lerpAmount;
    temp.y += (destY - temp.y) * lerpAmount;
    printf("%f;%f;\n", temp.x, temp.y);

    return temp;
}