#pragma once
#include "raylib.h"
#include "levelStructs.h"

Vector2 boxToVec2(BoxCollider2D box){
    return (Vector2){box.x, box.y};
}
Rectangle boxToRec(BoxCollider2D box){
    return (Rectangle){box.x, box.y, box.sizeX, box.sizeY};
}