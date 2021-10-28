#include "raylib.h"

//RecPP.h version 1.1

Rectangle addVec2ToRec(Rectangle rec, Vector2 vec2){
    Rectangle temp = rec;
    temp.x += vec2.x;
    temp.y += vec2.y;
    return temp;
}

Rectangle newRec(int x, int y, int sizeX, int sizeY){
    Rectangle temp;
    temp.x = x;
    temp.y = y;
    temp.width = sizeX;
    temp.height = sizeY;
    return temp;
}

Rectangle addRec(Rectangle rec1, Rectangle rec2, bool addSize){
    Rectangle temp;
    temp.x = rec1.x + rec2.x;
    temp.y = rec1.y + rec2.y;
    if(!addSize){
        return temp;
    }else{
        temp.width = rec1.width + rec2.width;
        temp.height = rec1.height + rec2.height;
        return temp;
    }
}

Rectangle resizeRec(Rectangle input, float scale){
    Rectangle temp;
    temp.x = input.x * scale;
    temp.y = input.y * scale;
    temp.width = input.width * scale;
    temp.height = input.height * scale;
    return temp;
}

Rectangle combineVec2(Vector2 position, Vector2 size){
    return (Rectangle){position.x, position.y, size.x, size.y};
}

bool isPointInRec(Rectangle rec, Vector2 point){
    return point.x >= rec.x && point.y >= rec.y && point.x <= rec.width + rec.x && point.y <= rec.height + rec.y;
}