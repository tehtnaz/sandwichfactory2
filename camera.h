#include "include/4.0/raylib.h"
#include "vector2pp.h"
#include <stdio.h>
#define CAM_ZERO (s_Camera){0,0,0,0,0,0}
#pragma once

//camera.h version 1

#ifndef CAMERA_H
    #define CAMERA_H

typedef struct Camera{
    Vector2 position;
    Vector2 maxCamera;
    Vector2 minCamera;
    bool smoothingEnabled;
    float smoothing;
    bool boundCam;
}s_Camera;

s_Camera checkBoundaries(s_Camera input){
    s_Camera temp = input;
    if(temp.position.x > temp.maxCamera.x){
        temp.position.x = temp.maxCamera.x;
    }else if(temp.position.x < temp.minCamera.x){
        temp.position.x = temp.minCamera.x;
    }
    if(temp.position.y > temp.maxCamera.y){
        temp.position.y = temp.maxCamera.y;
    }else if(temp.position.y < temp.minCamera.y){
        temp.position.y = temp.minCamera.y;
    }
    return temp;
}

s_Camera moveCamera(s_Camera input, float moveX, float moveY){
    s_Camera temp = input;
    temp.position.x += moveX;
    temp.position.y += moveY;
    
    if(temp.boundCam) temp = checkBoundaries(temp);

    return temp;
}

s_Camera setCamera(s_Camera input, float setX, float setY){
    s_Camera temp = input;
    temp.position.x = setX;
    temp.position.y = setY;
    
    if(temp.boundCam) temp = checkBoundaries(temp);
    
    return temp;
}

s_Camera lerpCamera(s_Camera input, float destX, float destY){
    if(input.smoothingEnabled == false){
        perror("Smoothing Disabled while trying to lerp camera!\n");
        return input;
    }
    
    s_Camera temp = input;

    temp.position = lerpVec2(temp.position, destX, destY, temp.smoothing);
    if(temp.boundCam) temp = checkBoundaries(temp);
    
    return temp;
}

#endif