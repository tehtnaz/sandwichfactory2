#pragma once
#include "raylib.h"
#include "levelStructs.h"

Vector2 boxToVec2(BoxCollider2D box){
    return (Vector2){box.x, box.y};
}
Rectangle boxToRec(BoxCollider2D box){
    return (Rectangle){box.x, box.y, box.sizeX, box.sizeY};
}
bool pointInBox(Vector2 point, BoxCollider2D box){
    return (
        point.x >= box.x &&
        point.x <= box.x + box.sizeX &&
        point.y >= box.y &&
        point.y <= box.y + box.sizeY
        );
}