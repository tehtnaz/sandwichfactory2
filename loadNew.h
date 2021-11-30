#include "dataHandling.h"
#include <stdio.h>
#include <stdlib.h>
int loadNew(int selectedLevel, bool choice, 
    char levelImagePath[64],
    Vector2* startingPos, Vector2* startingPos2,
    BoxCollider2D Col[30], TextBox levelText[2], PhysicsObject crate[2], Triangle triCol[10], BoxCollider2D ladderCol[5],
    int* colliderNum, int* textNum, int* ladderNum, int* crateNum,
    int* leverNum, int* doorNum, int* isMultiplayer, int* portalNum, int wallTags[16], int* wallNum, BoxCollider2D* goal
){
    char str[100];
    int temp = 1;
    s_Camera* nullCam = malloc(sizeof(s_Camera));
    //BoxCollider2D nullLadders[2];
    if(choice){
        while(temp == 1 && choice){
            printf("Enter path to .sf file(must be placed in custom_levels folder): ");
            char temp2[73];
            scanf("%s", temp2);
            sprintf(str, "custom_levels/%s", temp2);
            temp = readFileSF(str, true, levelImagePath, nullCam, startingPos, startingPos2, Col, ladderCol, levelText, crate, triCol, textNum, colliderNum, ladderNum, crateNum, leverNum, doorNum, isMultiplayer, portalNum, wallTags, wallNum, goal);
            if(temp == 1){
                printf("File could not be opened. Try another file.\n");
            }
        }
    }else{
        *leverNum = false;
        *doorNum = false;
        levelImagePath[0] = '\0';
        sprintf(str, "resources/levels/%d.sf", selectedLevel + 1);
        temp = readFileSF(str, false, levelImagePath, nullCam, startingPos, startingPos2, Col, ladderCol, levelText, crate, triCol, textNum, colliderNum, ladderNum, crateNum, leverNum, doorNum, isMultiplayer, portalNum, wallTags, wallNum, goal);
        printf("loadNew: test: %d\n", levelImagePath[0]);
    }
    return temp;
}

/*int loadNewCustom(int selectedLevel,
    char levelImagePath[64],
    Vector2* startingPos, Vector2* startingPos2,
    BoxCollider2D Col[15], TextBox levelText[2], PhysicsObject crate[2], Triangle triCol[10],
    int* colliderNum, int* textNum, int* ladderNum, int* crateNum,
    int* leverNum, int* doorNum, int* isMultiplayer,
    char levelPath[64]
){
    char str[100];
    int temp = 1;
    s_Camera* nullCam = malloc(sizeof(s_Camera));
    BoxCollider2D nullLadders[2];
    sprintf(str, "custom_levels/%s", levelPath);
    temp = readFileSF(str, true, levelImagePath, nullCam, startingPos, startingPos2, Col, nullLadders, levelText, crate, triCol, textNum, colliderNum, ladderNum, crateNum, leverNum, doorNum, isMultiplayer, portalNum);
    if(temp == 1){
        printf("File could not be opened. Try another file.\n");
    }
    return temp;
}*/