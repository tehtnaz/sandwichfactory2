#pragma once
#include "levelStructs.h"

void assign(){
    levelCol = 11;
    objectCol = 0;
    ladderNum = 0;
    disableCam = true;

    /*debug
    camera.maxCamera.x = 100000;
    camera.maxCamera.y = 100000;
    camera.minCamera.x = -100000;
    camera.minCamera.y = -100000;
    camera.smoothingEnabled = true;
    camera.smoothing = 0.25f;*/

    //levelID = 0;

    Col[0].x = 0;
    Col[0].y = 0;
    Col[0].sizeX = 240;
    Col[0].sizeY = 55;
    Col[0].trigger = false;
    Col[0].enabled = true;
    Col[0].ladder = false;

    Col[1].x = 0;
    Col[1].y = 119;
    Col[1].sizeX = 240;
    Col[1].sizeY = 16;
    Col[1].trigger = false;
    Col[1].enabled = true;
    Col[1].ladder = false;

    Col[2].x = 0;
    Col[2].y = 95;
    Col[2].sizeX = 16;
    Col[2].sizeY = 24;
    Col[2].trigger = false;
    Col[2].enabled = true;
    Col[2].ladder = false;

    Col[3].x = 32;
    Col[3].y = 104;
    Col[3].sizeX = 32;
    Col[3].sizeY = 15;
    Col[3].trigger = false;
    Col[3].enabled = true;
    Col[3].ladder = false;
    
    Col[4].x = 96;
    Col[4].y = 104;
    Col[4].sizeX = 32;
    Col[4].sizeY = 15;
    Col[4].trigger = false;
    Col[4].enabled = true;
    Col[4].ladder = false;

    Col[5].x = 112;
    Col[5].y = 89;
    Col[5].sizeX = 16;
    Col[5].sizeY = 15;
    Col[5].trigger = false;
    Col[5].enabled = true;
    Col[5].ladder = false;

    Col[6].x = 160;
    Col[6].y = 104;
    Col[6].sizeX = 48;
    Col[6].sizeY = 15;
    Col[6].trigger = false;
    Col[6].enabled = true;
    Col[6].ladder = false;

    Col[7].x = 176;
    Col[7].y = 89;
    Col[7].sizeX = 32;
    Col[7].sizeY = 15;
    Col[7].trigger = false;
    Col[7].enabled = true;
    Col[7].ladder = false;

    Col[8].x = 192;
    Col[8].y = 74;
    Col[8].sizeX = 16;
    Col[8].sizeY = 15;
    Col[8].trigger = false;
    Col[8].enabled = true;
    Col[8].ladder = false;

    Col[9].x = 224;
    Col[9].y = 104;
    Col[9].sizeX = 16;
    Col[9].sizeY = 15;
    Col[9].trigger = false;
    Col[9].enabled = true;
    Col[9].ladder = false;

    Col[10].x = 224;
    Col[10].y = 55;
    Col[10].sizeX = 16;
    Col[10].sizeY = 28;
    Col[10].trigger = false;
    Col[10].enabled = true;
    Col[10].ladder = false;

    startingPos.x = 4;
    startingPos.y = 68;

    levelTexts = 1;
    levelText[0].x = 26;
    levelText[0].y = 22;
    levelText[0].colour = WHITE;
    levelText[0].size = 11;
    sprintf(&levelText[0].text[0], "Use WASD or Arrow Keys to move");
    
    isLever = false;
    isDoor = false;
}