#include "raylib.h"
#include "checkColliders.h"

#ifndef TRIANGLE_H
    #define TRIANGLE_H

typedef struct startEnd{
    float x1;
    float x2;
}startEnd;

typedef struct Triangle{
    Vector2 p1;
    Vector2 p2;
    Vector2 anglePoint;
}Triangle;

typedef struct TriSlope{
    Vector2 p3;
    float slope;
    float init;
}TriSlope;

typedef struct TriColInfo{
    bool topLeft;
    bool topRight;
    bool botLeft;
    bool botRight;
    float floor;
}TriColInfo;

//collision checking
bool isPointInTri(Vector2 point, TriSlope tri);
TriColInfo isRecInTri(Rectangle rec, TriSlope tri);
Vector2 modifyPlayerWithTriCol(Rectangle player, TriSlope tri, TriColInfo info, float frictionCoefficient, float gravity, float* velocity);
Vector2 moveGetPlayerInput(Rectangle player, TriSlope tri, float characterSpeed, float* velocity, TriColInfo* info, bool isLeft, bool isRight, CollisionInfo* colInfo);
bool isUp(TriSlope tri);

//conversion
TriSlope triToTriSlope(Triangle tri);
Triangle triSlopeToTri(TriSlope tri);

//drawing (obv lol)
void drawSlope(float slope, float init);
void drawSlopeEx(float slope, float init, Color up, Color down);
void drawTriSlope(TriSlope triS, Color color);
void drawTri(Triangle tri, Color color);

#endif