#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "levelStructs.h"
#include "raylib.h"

//#define DEBUG_DATA_HANDLING
#pragma once

// dataHandling v3 (proper tokenization, complete rewrite, v2 completely thrown out)

int parseInt(const char* input);

int readFileSF(char path[128], 
            char levelImagePath[64], char backgroundPath[64],
            Vector2* startingPos, Vector2* startingPos2,
            BoxCollider2D levelCol[64], BoxCollider2D ladders[16], TextBox texts[2], PhysicsObject physobjects[8],
            int* levelTexts, int* levelColNum, int* ladderNum, int* physObjNum,
            int* isLever, int* isDoor, int* isMultiplayer, int* portalNum,
            int wallTags[16], int* wallNum, uint16_t* wallEnabled,
            BoxCollider2D* goal, int* scrollType, uint64_t* leverFlip);
