#include "raylib.h"
#include "triangles.h"
#include "checkColliders.h"
#include <stdio.h>

bool isPointInTri(Vector2 point, TriSlope tri){
    if(tri.p3.y > tri.p3.x * tri.slope + tri.init){
        //isUp
        if(point.y >= point.x * tri.slope + tri.init && point.y <= tri.p3.y){
            float x2 = (point.y - tri.init) / tri.slope;
            //DrawCircle(x2, point.y, 3, VIOLET);
            if(x2 > point.x){
                return point.x >= tri.p3.x && point.x <= x2;
            }else{
                return point.x <= tri.p3.x && point.x >= x2;
            }
        }else{
            return false;
        }
    }else{
        //isDown
        if(point.y <= point.x * tri.slope + tri.init && point.y >= tri.p3.y){
            float x2 = (point.y - tri.init) / tri.slope;
            //DrawCircle(x2, point.y, 3, VIOLET);
            if(x2 > point.x){
                return point.x >= tri.p3.x && point.x <= x2;
            }else{
                return point.x <= tri.p3.x && point.x >= x2;
            }
        }else{
            return false;
        }
    }
    return false;
}

TriColInfo isRecInTri(Rectangle rec, TriSlope tri){
    TriColInfo temp = {false, false, false, false, -1};
    if(isPointInTri((Vector2){rec.x, rec.y}, tri)){
        temp.topLeft = true;
    }
    if(isPointInTri((Vector2){rec.x + rec.width, rec.y}, tri)){
        temp.topRight = true;
    }
    if(isPointInTri((Vector2){rec.x, rec.y + rec.height}, tri)){
        temp.botLeft = true;
    }
    if(isPointInTri((Vector2){rec.x + rec.width, rec.y + rec.height}, tri)){
        temp.botRight = true;
    }
    if(temp.botLeft || temp.botRight){
        temp.floor = rec.y;
        printf("what");
    }else{
        temp.floor = -1;
    }
    return temp;
}

// move player based on which tri they interacted with
Vector2 modifyPlayerWithTriCol(Rectangle player, TriSlope tri, TriColInfo info, float frictionCoefficient, float gravity, float* velocity){
    float fixedVelocity = -gravity;
    
    Vector2 returnVec2 = (Vector2){player.x, player.y};
    if(info.botRight && tri.slope < 0 && tri.init >= 0 && fixedVelocity > 0){
        // go left from bottom
        returnVec2.x = ((player.y + player.height - tri.init + fixedVelocity) / tri.slope) - player.width;
        //returnVec2.y += velocity;
        *velocity = 0;
        printf("teleporting from ?(%f, %f) to ?(%f, %f). s: %f  i: %f psX: %f psY: %f v:%f\n", player.x, player.y, returnVec2.x, returnVec2.y, tri.slope, tri.init, player.width, player.height, fixedVelocity);
    }else if(info.topRight && tri.slope > 0 && tri.init <= 0 && fixedVelocity < 0){
        // go left from top
        returnVec2.x = ((player.y - tri.init + fixedVelocity) / tri.slope) - player.width;
        *velocity = 0;
        printf("teleporting from ?(%f, %f) to ?(%f, %f). s: %f  i: %f psX: %f psY: %f v:%f\n", player.x, player.y, returnVec2.x, returnVec2.y, tri.slope, tri.init, player.width, player.height, fixedVelocity);
    }else if(info.botLeft && tri.slope > 0 && tri.init >= 0 && fixedVelocity > 0){
        // go right from bottom
        returnVec2.x = (player.y + player.height - tri.init + fixedVelocity) / tri.slope;
        //returnVec2.y += velocity;
        *velocity = 0;
        printf("teleporting from ?(%f, %f) to ?(%f, %f). s: %f  i: %f psX: %f psY: %f v:%f\n", player.x, player.y, returnVec2.x, returnVec2.y, tri.slope, tri.init, player.width, player.height, fixedVelocity);
    }else if(info.topLeft && tri.slope < 0 && tri.init <= 0 && fixedVelocity < 0){
        // go right from top
        returnVec2.x = (player.y - tri.init + fixedVelocity) / tri.slope;
        *velocity = 0;
        printf("teleporting from ?(%f, %f) to ?(%f, %f). s: %f  i: %f psX: %f psY: %f v:%f\n", player.x, player.y, returnVec2.x, returnVec2.y, tri.slope, tri.init, player.width, player.height, fixedVelocity);
    }else{
        //is not sliding on angled surface.
        printf("player is not sliding on surface\n");
    }
    return returnVec2;
}

Vector2 moveGetPlayerInput(Rectangle player, TriSlope tri, float characterSpeed, float* velocity, TriColInfo* info, bool isRight, bool isLeft, CollisionInfo* colInfo){
    Vector2 returnVec2 = (Vector2){player.x, player.y};
    float fixedVelocity = -*velocity;
    //TriColInfo info = isRecInTri(player, tri);
    if(info->botRight && tri.slope < 0 && tri.init >= 0 && fixedVelocity > 0){
        // go left from bottom
        if(isRight){
            returnVec2.y = (tri.slope * (player.x + characterSpeed)) + tri.init - player.height;
            info->floor = returnVec2.y;
        //printf("teleporting from ?(%f, %f) to ?(%f, %f). s: %f  i: %f psX: %f psY: %f v:%f\n", player.x, player.y, returnVec2.x, returnVec2.y, tri.slope, tri.init, player.width, player.height, fixedVelocity);

        }else{
            returnVec2.y = (tri.slope * (player.x)) + tri.init - player.height;
            info->floor = returnVec2.y;

        }
        printf("teleporting from ?(%f, %f) to ?(%f, %f). s: %f  i: %f psX: %f psY: %f v:%f\n", player.x, player.y, returnVec2.x, returnVec2.y, tri.slope, tri.init, player.width, player.height, fixedVelocity);

        colInfo->down = true;
        //returnVec2.y += velocity;
    
    }else if(info->topRight && tri.slope > 0 && tri.init <= 0 && fixedVelocity < 0){
        // go left from top
        if(isRight){
            returnVec2.y = (tri.slope * (player.x + characterSpeed)) + tri.init;
            info->floor = returnVec2.y;
        printf("teleporting from ?(%f, %f) to ?(%f, %f). s: %f  i: %f psX: %f psY: %f v:%f\n", player.x, player.y, returnVec2.x, returnVec2.y, tri.slope, tri.init, player.width, player.height, fixedVelocity);

        }else{
            returnVec2.y = (tri.slope * (player.x)) + tri.init - player.height;
            info->floor = returnVec2.y;

        }
        colInfo->right = true;
        colInfo->up = true;
    
    }else if(info->botLeft && tri.slope > 0 && tri.init >= 0 && fixedVelocity > 0){
        // go right from bottom
        if(isLeft){
            returnVec2.y = (tri.slope * (player.x - characterSpeed)) + tri.init - player.height;
            info->floor = returnVec2.y;
        printf("teleporting from ?(%f, %f) to ?(%f, %f). s: %f  i: %f psX: %f psY: %f v:%f\n", player.x, player.y, returnVec2.x, returnVec2.y, tri.slope, tri.init, player.width, player.height, fixedVelocity);

        }else{
            returnVec2.y = (tri.slope * (player.x)) + tri.init - player.height;
            info->floor = returnVec2.y;

        }
        colInfo->down = true;
        //returnVec2.y += velocity;
    
    }else if(info->topLeft && tri.slope < 0 && tri.init <= 0 && fixedVelocity < 0){
        // go right from top
        if(isLeft){
            returnVec2.y = (tri.slope * (player.x - characterSpeed)) + tri.init;
            info->floor = returnVec2.y;
        printf("teleporting from ?(%f, %f) to ?(%f, %f). s: %f  i: %f psX: %f psY: %f v:%f\n", player.x, player.y, returnVec2.x, returnVec2.y, tri.slope, tri.init, player.width, player.height, fixedVelocity);

        }else{
            returnVec2.y = (tri.slope * (player.x)) + tri.init - player.height;
            info->floor = returnVec2.y;

        }
        colInfo->left = true;
        colInfo->up = true;
        
    
    }else{
        //is not sliding on angled surface.
        //printf("player is not sliding on surface\n");
        //printf("infof: %f", info->floor);
    }
    return returnVec2;
}

/*CollisionInfo triColInfoToColInfo(TriColInfo info){
    if(info.botRight || info.)
}*/


bool isUp(TriSlope tri){
    if(tri.p3.y > tri.p3.x * tri.slope + tri.init){
        return true;
    }else{
        return false;
    }
}

TriSlope triToTriSlope(Triangle tri){
    TriSlope temp;
    temp.p3 = tri.anglePoint;
    temp.slope = (tri.p1.y - tri.p2.y) / (tri.p1.x - tri.p2.x);
    temp.init = tri.p1.y - (tri.p1.x * temp.slope);
    return temp;
}

void drawSlope(float slope, float init){
    Vector2 vect1;
    Vector2 vect2;

    vect1.y = init;

    vect2.x = -init / slope;
    vect2.y = 0;
    DrawLineEx(vect1, vect2, 5, GREEN);
}

Triangle triSlopeToTri(TriSlope tri){
    Triangle temp;

    temp.anglePoint = tri.p3;

    temp.p1.y = tri.p3.y;
    temp.p1.x = (temp.p1.y - tri.init) / tri.slope;

    temp.p2.x = tri.p3.x;
    temp.p2.y = (tri.slope * temp.p2.x) + tri.init;

    return temp;
}

void drawSlopeEx(float slope, float init, Color up, Color down){
    Vector2 vect1;
    Vector2 vect2;

    vect1.x = 0;
    vect1.y = init;
    if(slope > 0){

        vect2.x = (1080 - init) / slope;
        vect2.y = 1080;

    }else{

        vect2.x = -init / slope;
        vect2.y = 0;
    }

    
    if(isUp((TriSlope){(Vector2){GetMouseX(),GetMouseY()}, slope, init}))
        DrawLineEx(vect1, vect2, 5, up);
    else
        DrawLineEx(vect1, vect2, 5, down);

    //printf("p1.x: %f; p1.y %f; p2.x: %f; p2.y: %f; s: %f\n", vect1.x, vect1.y, vect2.x, vect2.y, slope);
}

void drawTriSlope(TriSlope triS, Color color){
    Triangle temp;
    temp = triSlopeToTri(triS);

    DrawTriangle(temp.p1, temp.p2, temp.anglePoint, color);

}

void drawTri(Triangle tri, Color color){
    DrawTriangle(tri.p1, tri.p2, tri.anglePoint, color);
}