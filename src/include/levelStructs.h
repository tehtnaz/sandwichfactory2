#include "raylib.h"
#include <stdint.h>
#pragma once

// levelstructs.h version 1.1
// trigger bool -> int

typedef struct BoxCollider2D{
    int x;
    int y;
    int sizeX;
    int sizeY;
    uint32_t trigger;
    bool ladder; // private
    bool enabled;
}BoxCollider2D;

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