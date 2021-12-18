#include "dataHandling.h"
#include <stdio.h>
#include <stdlib.h>

void EndAtExtension(char* input){
    int i = 0;
    while(input[i] != '\0' && input[i] != '.'){
        i++;
    }
    if(input[i] == '.'){
        input[i] = '\0';
    }else{
        printf("WARNING: EndAtExtension - Could not find period before end of string");
    }
}

int loadNew(int selectedLevel, bool custom, 
    char levelImagePath[64], char backgroundPath[64],
    Vector2* startingPos, Vector2* startingPos2,
    BoxCollider2D Col[64], TextBox levelText[2], PhysicsObject crate[8], Triangle triCol[10], 
    BoxCollider2D ladderCol[16],
    int* colliderNum, int* textNum, int* ladderNum, int* crateNum,
    int* leverNum, int* doorNum, int* isMultiplayer, int* portalNum, int wallTags[16], int* wallNum, uint16_t* wallEnabled,
    BoxCollider2D* goal, int* scrollType, uint64_t* leverFlip, char path[128]
){
    char str[128];
    int temp = 1;
    //s_Camera* nullCam = malloc(sizeof(s_Camera));
    //BoxCollider2D nullLadders[2];
    if(custom){
        if(selectedLevel == 1){
            while(temp == 1 && custom){
                printf("Enter path to .sf file or a folder (must be placed in custom_levels folder): ");
                char temp2[110];
                scanf("%s", temp2);
                if(temp2[TextLength(temp2) - 1] == '/') sprintf(str, "custom_levels/%s1.sf", temp2);
                else sprintf(str, "custom_levels/%s", temp2);
                printf("%s\n",temp2);
                temp = readFileSF(str, levelImagePath, backgroundPath, startingPos, startingPos2, Col, ladderCol, levelText, crate, triCol, textNum, colliderNum, ladderNum, crateNum, leverNum, doorNum, isMultiplayer, portalNum, wallTags, wallNum, wallEnabled, goal, scrollType, leverFlip);
                if(temp == 1){
                    printf("File could not be opened. Try another file.\n");
                }else{
                    if(temp2[TextLength(temp2) - 1] != '/' && temp2[TextLength(temp2) - 1] != '\\') EndAtExtension(temp2);
                    temp = TextCopy(path, temp2);
                    if(temp == 1) printf("ERROR: Failed text copy");
                }
            }
        }else{
            sprintf(str, "custom_levels/%s%d.sf", path, selectedLevel);
            temp = readFileSF(str, levelImagePath, backgroundPath, startingPos, startingPos2, Col, ladderCol, levelText, crate, triCol, textNum, colliderNum, ladderNum, crateNum, leverNum, doorNum, isMultiplayer, portalNum, wallTags, wallNum, wallEnabled, goal, scrollType, leverFlip);
            if(temp == 1){
                printf("ERROR: loadNew - Error detected in readFileSF. Ending function.\n");
                return 1;
            }
        }
    }else{
        *leverNum = false;
        *doorNum = false;
        levelImagePath[0] = '\0';
        sprintf(str, "resources/levels/%d.sf", selectedLevel + 1);
        temp = readFileSF(str, levelImagePath, backgroundPath, startingPos, startingPos2, Col, ladderCol, levelText, crate, triCol, textNum, colliderNum, ladderNum, crateNum, leverNum, doorNum, isMultiplayer, portalNum, wallTags, wallNum, wallEnabled, goal, scrollType, leverFlip);
        printf("loadNew: test: %d\n", levelImagePath[0]);
    }
    return temp;
}