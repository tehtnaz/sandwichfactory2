#pragma once
#include "levelStructs.h"

void assign4(){
    levelCol = 4;
    objectCol = 0;
    ladderNum = 1;
    disableCam = true;
    //levelID = 1;

    Col[0].x = 0;
    Col[0].y = 0;
    Col[0].sizeX = 240;
    Col[0].sizeY = 83;
    Col[0].trigger = false;
    Col[0].enabled = true;

    Col[1].x = 0;
    Col[1].y = 104;
    Col[1].sizeX = 16;
    Col[1].sizeY = 31;
    Col[1].trigger = false;
    Col[1].enabled = true;

    Col[2].x = 16;
    Col[2].y = 119;
    Col[2].sizeX = 224;
    Col[2].sizeY = 16;
    Col[2].trigger = false;
    Col[2].enabled = true;

    Col[3].x = 224;
    Col[3].y = 83;
    Col[3].sizeX = 16;
    Col[3].sizeY = 15;
    Col[3].trigger = false;
    Col[3].enabled = true;

    ladderCol[0].x = 82;
    ladderCol[0].y = 83;
    ladderCol[0].sizeX = 7;
    ladderCol[0].sizeY = 36;

    startingPos.x = 4;
    startingPos.y = 81;

    levelTexts = 2;
    levelText[0].x = 45;
    levelText[0].y = 22;
    levelText[0].colour = WHITE;
    levelText[0].size = 11;
    sprintf(&levelText[0].text[0], "Hold E to grab");

    levelText[1].x = 60;
    levelText[1].y = 34;
    levelText[1].colour = WHITE;
    levelText[1].size = 11;
    sprintf(&levelText[1].text[0], "crates.");

    isLever = false;
    isDoor = false;
}