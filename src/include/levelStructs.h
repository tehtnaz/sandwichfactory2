#include "raylib.h"
#include <stdint.h>
#pragma once

// levelstructs.h version 1.2
// 1.1: trigger bool -> int
// 1.2: addded tag

typedef struct BoxCollider2D{
    int x;
    int y;
    int sizeX;
    int sizeY;
    int trigger; // 0 = none, anything else activates a group
    bool ladder; // private
        //could replace with itemType int
        // 0 = default
        // 1 = ladder
        // 2 = door
        // 3 = lever (unneeded since it'd just have a trigger anyway)
        // 4 = level Win trigger
    bool enabled;
    int tag; // 0 = default (never changed), anything else can be activated
}BoxCollider2D;

//levers = bc2d - use trigger to check for which doors to switch
//doors = bc2d - use tag to go according to levers - use collision enabled to judge whether door is opened
//portal = bc2d - use trigger to check for which walls to deactivate
//col / ladder = bc2d - use tag to check for portals


typedef struct TextBox{
    int x;
    int y;
    int size;
    char text[40];
    Color colour;
}TextBox;

typedef struct PhysicsObject{
    Vector2 position;
    int sizeX;
    int sizeY;
    Vector2 velocity; // private
    bool trigger;
    bool enabled;
    bool isTouchingPlayer; // private
}PhysicsObject;