#pragma once
#include "levelStructs.h"

void assign2(){
    levelCol = 5;
    objectCol = 0;
    ladderNum = 0;
    disableCam = true;
    //levelID = 1;

    Col[0].x = 0;
    Col[0].y = 0;
    Col[0].sizeX = 240;
    Col[0].sizeY = 71;
    Col[0].trigger = false;
    Col[0].enabled = true;

    Col[1].x = 0;
    Col[1].y = 95;
    Col[1].sizeX = 16;
    Col[1].sizeY = 40;
    Col[1].trigger = false;
    Col[1].enabled = true;

    Col[2].x = 16;
    Col[2].y = 119;
    Col[2].sizeX = 224;
    Col[2].sizeY = 16;
    Col[2].trigger = false;
    Col[2].enabled = true;

    Col[3].x = 121;
    Col[3].y = 104;
    Col[3].sizeX = 119;
    Col[3].sizeY = 15;
    Col[3].trigger = false;
    Col[3].enabled = true;

    Col[4].x = 224;
    Col[4].y = 71;
    Col[4].sizeX = 16;
    Col[4].sizeY = 12;
    Col[4].trigger = false;
    Col[4].enabled = true;

    //Lever
    Col[5].x = 14;
    Col[5].y = 99;
    Col[5].sizeX = 16;
    Col[5].sizeY = 16;
    Col[5].trigger = true;
    Col[5].enabled = true;

    //Door
    Col[6].x = 226;
    Col[6].y = 83;
    Col[6].sizeX = 7;
    Col[6].sizeY = 21;
    Col[6].trigger = false;
    Col[6].enabled = true;

    startingPos.x = 4;
    startingPos.y = 72;

    levelTexts = 2;
    levelText[0].x = 45;
    levelText[0].y = 22;
    levelText[0].colour = WHITE;
    levelText[0].size = 11;
    sprintf(&levelText[0].text[0], "Press E to toggle switches.");

    levelText[1].x = 60;
    levelText[1].y = 34;
    levelText[1].colour = WHITE;
    levelText[1].size = 11;
    sprintf(&levelText[1].text[0], "Switches open doors.");

    isLever = true;
    isDoor = true;
}