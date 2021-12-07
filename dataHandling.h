#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "levelStructs.h"
#include "compareString.h"
#include "camera.h"
#include "triangles.h"
#pragma once


// dataHandling v2

//1.1
// added sp2 property.
    // it checks for the second spawnpoint

//2
// i added enough that i dont remember


//File Structure
//    data;data;data;...;!
//    ! = terminates the file

int parseInt(char input[11], int arraySize){ // Sometimes while parsing garbage data could be given (input: "128;ns" however still outputs to 128)
    char ch = '?';
    int output = 0;
    int i = 0;

    bool isEnd = false;

    if(arraySize == 0){
        printf("Warning: parseInt - tried to parse 0 characters");
        return 0;
    }
    if(arraySize > 10){
        printf("ERROR: parseInt - output int would exceed 32bit number limit");
        return 0;
    }

    for(; i < 11 && !isEnd && i < arraySize; i++){ // Warning: I left this as a for statement while not declaring a variable because I'm a lazy bastard
        ch = input[i];

        switch(ch){
            //case '-':
                //output = -output;
                //break;
            case '0':
                output = output * 10;
                break;
            case '1':
                output = (output * 10) + 1;
                break;
            case '2':
                output = (output * 10) + 2;
                break;
            case '3':
                output = (output * 10) + 3;
                break;
            case '4':
                output = (output * 10) + 4;
                break;
            case '5':
                output = (output * 10) + 5;
                break;
            case '6':
                output = (output * 10) + 6;
                break;
            case '7':
                output = (output * 10) + 7;
                break;
            case '8':
                output = (output * 10) + 8;
                break;
            case '9':
                output = (output * 10) + 9;
                break;
            case '!':
            case ';':
            case ',':
            case '\0':
                isEnd = true;
                break;
            case '.':
                printf("ERROR: parseInt - Received \'.\' as input. This will fail because the parser checks for integers. Ending function...\n");
                isEnd = true;
                output = 0;
                break;
            default:
                printf("ERROR: parseInt - Received non-number as input. Ending function...\n");
                isEnd = true;
                output = 0;
                break;
        }
        //printf("%c + %d\n", ch, output);
    }
    //printf("final char + output: %c + %d\n", ch, output);
    if(!isEnd && i != arraySize){
        printf("ERROR: parseInt - Reached end of parse before termination.  ");
        printf("Given Max size: %d\n", arraySize);
        
    }
    printf("parseInt: Given string: %s", input);
    printf(" - Output: %d\n", output);
    return output;
}

float parseFloat(char input[12]){
    char ch;
    float output = 0;

    int decimal = 0;
    bool isEnd = false;

    for(int i = 0; i < 12 && !isEnd; i++){
        ch = input[i];

        if(decimal != 0 && ch != '!' && ch != ';' && ch != ')' && ch != ','){
            decimal++;
        }
        switch(ch){
            case '0':
                output = output * 10;
                break;
            case '1':
                output = (output * 10) + 1;
                break;
            case '2':
                output = (output * 10) + 2;
                break;
            case '3':
                output = (output * 10) + 3;
                break;
            case '4':
                output = (output * 10) + 4;
                break;
            case '5':
                output = (output * 10) + 5;
                break;
            case '6':
                output = (output * 10) + 6;
                break;
            case '7':
                output = (output * 10) + 7;
                break;
            case '8':
                output = (output * 10) + 8;
                break;
            case '9':
                output = (output * 10) + 9;
                break;
            case '!':
            case ';':
            case ',':
            case ')':
                isEnd = true;
                break;
            case '.':
                decimal++;
                break;
        }
        //printf("%c + %f\n", ch, output);
    }
    for(int i = 0; i < decimal - 1; i++){
        output /= 10;
        printf("0.1 ^ %d * %f\n", i, output);
    }
    

    if(!isEnd){
        printf("ERROR: parseFloat - Reached end of parse before termination");
        
    }
    printf("parseFloat: Given string: %s - Output: %f. Watch out for float precision issues\n", input, output);
    return output;
}

//Input structure (data,data) or data,data) aka: does not require starting bracket
Vector2 readVector2(char input[20]){
    char ch;
    int inputSelect = 0;
    Vector2 output;
    char sendToParse[11];
    int i;

    printf("readVector2: Reading \"%s\"\n", input);

    ch = input[inputSelect];

    if(ch == '('){
        inputSelect++;
        ch = input[inputSelect];
    }

    if(ch == ',' || ch == ')'){
        inputSelect++;
        ch = input[inputSelect];
        output.x = 0;
    }else{
        i = 0;
        while(ch != ','){
            sendToParse[i] = ch;
            inputSelect++;
            ch = input[inputSelect];
            i++;
            //printf("%d-", i);
        }
        sendToParse[i] = ';';
        printf("readVector2: Item 1 length: %d\n", i);
        output.x = parseInt(sendToParse, i + 1);
        inputSelect++;
        ch = input[inputSelect];
    }

    if(ch == ')'){
        inputSelect++;
        ch = input[inputSelect];
        output.y = 0;
    }else{
        i = 0;
        while(ch != ')'){
            sendToParse[i] = ch;
            inputSelect++;
            ch = input[inputSelect];
            i++;
            //printf("%d-", i);
        }
        sendToParse[i] = ';';
        printf("readVector2: Item 2 length: %d\n", i);
        output.y = parseInt(sendToParse, i + 1);
        inputSelect++;
        ch = input[inputSelect];
    }

    return output;
}

Vector2 readVector2Float(char input[20]){
    char ch;
    int inputSelect = 0;
    Vector2 output;
    char sendToParse[11];
    int i;

    ch = input[inputSelect];

    if(ch == '('){
        inputSelect++;
        ch = input[inputSelect];
    }

    if(ch == ',' || ch == ')'){
        inputSelect++;
        ch = input[inputSelect];
        output.x = 0;
    }else{
        i = 0;
        while(ch != ','){
            sendToParse[i] = ch;
            inputSelect++;
            ch = input[inputSelect];
            i++;
        }
        sendToParse[i] = ';';
        printf("readVector2Float: Item 1 length: %d\n", i);
        output.x = parseFloat(sendToParse);
        inputSelect++;
        ch = input[inputSelect];
    }

    if(ch == ')'){
        inputSelect++;
        ch = input[inputSelect];
        output.y = 0;
    }else{
        i = 0;
        while(ch != ')'){
            sendToParse[i] = ch;
            inputSelect++;
            ch = input[inputSelect];
            i++;
        }
        sendToParse[i] = ';';
        printf("readVector2Float: Item 2 length: %d\n", i);
        output.y = parseFloat(sendToParse);
        inputSelect++;
        ch = input[inputSelect];
    }

    return output;
}

Color parseColor(char input[17], int inputSize){
    int select = 0;
    Color output;
    char sendToParse[3];
    int i;
    int temp;

    if(input[select] == '('){
        select++;
    }

    for(int s = 0; s < 4; s++){
        i = 0;
        while(input[select] != ',' && i < 4 && input[select] != ')'){
            //printf("select: %d, i: %d, char: %c\n", select, i, input[select]);
            sendToParse[i] = input[select];
            select++;
            i++;
        }
        select++;
        if(i > 3){
            printf("ERROR: parseColor - Value exceeds maximum (Overflowed. Missing comma or parenthesis)");
        }
        temp = parseInt(sendToParse, i);
        if(temp > 255){
            printf("ERROR: parseColor - Value exceeds maximum (Value larger than 255)");
            temp = 255;
        }
        switch(s){
            case 0:
                output.r = temp;
                break;
            case 1:
                output.g = temp;
                break;
            case 2:
                output.b = temp;
                break;
            case 3:
                output.a = temp;
                break;
        }
        //printf("i: %s o: %d, s: %d, select: %d\n", sendToParse, temp, s, select);
    }
    return output;

}

void readIntArray(char input[128], int output[16]){
    char ch;
    int inputSelect = 0;
    char sendToParse[11];
    int i;
    int objectSelect = 0;

    ch = input[inputSelect];

    if(ch == '('){
        inputSelect++;
        ch = input[inputSelect];
    }

    while(ch != ')'){
        if(ch == ','){
            inputSelect++;
            ch = input[inputSelect];
            output[objectSelect] = 0;
        }else{
            i = 0;
            while(ch != ',' && ch != ')'){
                sendToParse[i] = ch;
                inputSelect++;
                ch = input[inputSelect];
                i++;
            }
            sendToParse[i] = ';';
            output[objectSelect] = parseInt(sendToParse, i + 1);
            if(ch == ','){
                inputSelect++;
                ch = input[inputSelect];
            }
        }
        objectSelect++;
    }
    printf("readIntArray finished - given string: %s\n", input);
}

int readBoolArray(char input[16], bool output[8]){
    char ch;
    int inputSelect = 0;
    int objectSelect = 0;

    ch = input[inputSelect];

    if(ch == '('){
        inputSelect++;
        ch = input[inputSelect];
    }

    while(ch != ')'){
        if(objectSelect > 8){
            printf("ERROR: readBoolArray - Too much bools given or array could not be terminated");
            return 1;
        }
        if(ch == ','){
            inputSelect++;
            ch = input[inputSelect];
            output[objectSelect] = false;
        }else{
            if(ch == '0' || ch == 'f' || ch == 'F'){
                output[objectSelect] = false;
            }else if(ch == '1' || ch == 't' || ch == 'T'){
                output[objectSelect] = true;
            }else{
                printf("ERROR: readBoolArray - input is not bool");
                return 1;
            }
            inputSelect++;
            ch = input[inputSelect];
            if(ch != ','){
                if(ch != ')'){
                    printf("ERROR: readBoolArray - input is not bool");
                    return 1;
                }
            }else{
                inputSelect++;
                ch = input[inputSelect];
            }
        }
        objectSelect++;
    }
    return 0;
}

bool parseBool(char ch){
    if(ch == '0' || ch == 'f' || ch == 'F'){
        printf("parseBool: Returned false\n");
        return false;
    }else if (ch == '1' || ch == 't' || ch == 'T'){
        printf("parseBool: Returned true\n");
        return true;
    }else{
        printf("ERROR: parseBool - Input is not bool");
        return false;
    }
}

//Note: Does not read ladder property
//You MUST either give the script a inputSize or terminating character ( chars = ';' and ')' )
//Function accepts '(' as first character
BoxCollider2D parseBoxCollider(char input[128], int inputSize, bool isLadder){
    int select = 0;
    char sendToParse[11];
    int temp;
    int i;
    BoxCollider2D returnedCol;

    if(input[select] == '('){
        select++;
    }

    for(int propertySelect = 0; propertySelect < 4; propertySelect++){
        i = 0;
        temp = 0;
        while(input[select] != ','){
            sendToParse[i] = input[select];
            select++;
            i++;
        }
        select++;
        temp = parseInt(sendToParse, i);
        switch(propertySelect){
            case 0:
                returnedCol.x = temp;
                break;
            case 1:
                returnedCol.y = temp;
                break;
            case 2:
                returnedCol.sizeX = temp;
                break;
            case 3:
                returnedCol.sizeY = temp;
                break;
        }
    }
    i = 0;
    temp = 0;
    while(input[select] != ','){
        //printf("%c",input[select]);
        sendToParse[i] = input[select];
        select++;
        i++;
    }
    returnedCol.trigger = parseInt(sendToParse, i);
    
    //printf("\n");   
    select++;

    returnedCol.enabled = parseBool(input[select]);
    select++;
    if(input[select] == ','){
        select++;
    }else{
        printf("ERROR: parseBoxCollider2D - Input is not a bool\n");
    }
    i = 0;
    temp = 0;
    while(input[select] != ')'){
        //printf("%c",input[select]);
        sendToParse[i] = input[select];
        select++;
        i++;
    }
    //printf("\n");
    returnedCol.tag = parseInt(sendToParse, i);
    if(input[select] != ';' && input[select] != ')' && (select < inputSize - 1 || select > inputSize)){
        printf("ERROR: parseBoxCollider2D - Could not terminate Collider properly...\n\tNote: This is probably because a bool was incorrect\n\tch = %c (%d)\n", input[select], input[select]);
    }
    return returnedCol;
}

//Note: Does not read velocity or isTouchingPlayer
PhysicsObject parsePhysicsObject(char input[128], int inputSize){
    printf("parsePhyscisObject:  input: %s\n", input);
    int select = 0;
    char sendToParse[20];
    int temp;
    int i;
    PhysicsObject returnedObj;

    if(input[select] == '('){
        select++;
    }

    if(input[select] == '@'){
        // we have a vector2
        select++;
        i = 0;
        if(input[select] == 'f'){
            temp = 1;
            select++;
        }else{
            temp = 0;
        }
        while(input[select] != ')'){
            sendToParse[i] = input[select];
            select++;
            i++;
        }
        sendToParse[i] = input[select];
        if(temp == 0){
            returnedObj.position = readVector2(sendToParse);
        }else{
            returnedObj.position = readVector2Float(sendToParse);
        }
        
        select++;
        if(input[select] == ','){
            select++;
        }
    }else{
        // we have floats
        i = 0;
        while(input[select] != ','){
            sendToParse[i] = input[select];
            select++;
            i++;
        }
        sendToParse[i] = ';';
        returnedObj.position.x = parseFloat(sendToParse);
        select++;

        i = 0;
        while(input[select] != ','){
            sendToParse[i] = input[select];
            select++;
            i++;
        }
        sendToParse[i] = ';';
        returnedObj.position.y = parseFloat(sendToParse);
        select++;
    }

    i = 0;
    while(input[select] != ','){
        sendToParse[i] = input[select];
        select++;
        i++;
    }
    sendToParse[i] = ';';
    returnedObj.sizeX = parseInt(sendToParse, i);
    select++;

    i = 0;
    while(input[select] != ','){
        sendToParse[i] = input[select];
        select++;
        i++;
    }
    sendToParse[i] = ';';
    returnedObj.sizeY = parseInt(sendToParse, i);
    select++;

    returnedObj.trigger = parseBool(input[select]);
    select++;
    if(input[select] == ','){
        select++;
    }else{
        printf("ERROR: parsePhysicsObject - Input is not bool  ");
        printf("given char - %c\n", input[select]);
    }

    returnedObj.enabled = parseBool(input[select]);
    select++;
    if(input[select] == ')'){
        select++;
    }else{
        printf("ERROR: parsePhysicsObject - Input is not bool  ");
        printf("given char - %c\n", input[select]);
    }
    printf("parsePhysObj: input: %s\n", input);
    printf("parsePhysObj: output: %f, %f, %d, %d, %d, %d\n", returnedObj.position.x, returnedObj.position.y, returnedObj.sizeX, returnedObj.sizeY, returnedObj.trigger, returnedObj.enabled);
    return returnedObj;
}

Triangle parseTriangle(char input[128], int inputSize){
    printf("parseTriangle: input: %s\n", input);
    int select = 0;
    char sendToParse[20];
    int temp;
    int i;
    Triangle returnedTri;

    if(input[select] == '('){
        select++;
    }

    if(input[select] == '@'){
        // we have a vector2
        select++;
        i = 0;
        if(input[select] == 'f'){
            temp = 1;
            select++;
        }else{
            temp = 0;
        }
        while(input[select] != ')'){
            sendToParse[i] = input[select];
            select++;
            i++;
        }
        sendToParse[i] = input[select];
        if(temp == 0){
            returnedTri.p1 = readVector2(sendToParse);
        }else{
            returnedTri.p1 = readVector2Float(sendToParse);
        }
        
        select++;
        if(input[select] == ','){
            select++;
        }
    }else{
        // we have floats
        i = 0;
        while(input[select] != ','){
            sendToParse[i] = input[select];
            select++;
            i++;
        }
        sendToParse[i] = ';';
        returnedTri.p1.x = parseFloat(sendToParse);
        select++;

        i = 0;
        while(input[select] != ','){
            sendToParse[i] = input[select];
            select++;
            i++;
        }
        sendToParse[i] = ';';
        returnedTri.p1.y = parseFloat(sendToParse);
        select++;
    }

    i = 0;
    while(input[select] != ','){
        sendToParse[i] = input[select];
        select++;
        i++;
    }
    sendToParse[i] = ';';
    returnedTri.p2.x = parseFloat(sendToParse);
    select++;

    i = 0;
    while(input[select] != ','){
        sendToParse[i] = input[select];
        select++;
        i++;
    }
    sendToParse[i] = ';';
    returnedTri.p2.y = parseFloat(sendToParse);
    select++;

    i = 0;
    while(input[select] != ','){
        sendToParse[i] = input[select];
        select++;
        i++;
    }
    sendToParse[i] = ';';
    returnedTri.anglePoint.x = parseFloat(sendToParse);
    select++;

    i = 0;
    while(input[select] != ','){
        sendToParse[i] = input[select];
        select++;
        i++;
    }
    sendToParse[i] = ';';
    returnedTri.anglePoint.y = parseFloat(sendToParse);
    select++;


    printf("parseTriangle: input: %s\n", input);
    printf("parseTriangle: output: %f, %f, %f, %f, %f, %f\n", returnedTri.p1.x, returnedTri.p1.y, returnedTri.p2.x, returnedTri.p2.y, returnedTri.anglePoint.x, returnedTri.anglePoint.y);
    return returnedTri;
}

TextBox parseTextBox(char input[128], int inputSize){
    int select = 0;
    char sendToParse[17];
    int temp;
    int i;
    TextBox returnedTextBox;

    if(input[select] == '('){
        select++;
    }

    for(int s = 0; s < 3; s++){
        i = 0;
        while(input[select] != ','){
            sendToParse[i] = input[select];
            select++;
            i++;
        }
        temp = parseInt(sendToParse, i);
        select++;
        switch(s){
            case 0:
                returnedTextBox.x = temp;
                break;
            case 1:
                returnedTextBox.y = temp;
                break;
            case 2:
                returnedTextBox.size = temp;
                break;
        }
    }

    i = 0;
    while(input[select] != ',' && i < 40){
        if(input[select] == '\\'){
            temp = 1;
            //printf("-%c\n", input[select]);
        }
        if(temp != 1){
            returnedTextBox.text[i] = input[select];
        }
        //printf("-%c\n", input[select]);
        select++;
        i++;
        if(input[select] == ',' && temp == 1){
            i--;
            returnedTextBox.text[i] = input[select];
            //printf("-%c\n", input[select]);
            select++;
            i++;
            //printf("-%c\n", input[select]);
            temp = 0;
        }
    }
    select++;
    if(i == 40){
        printf("ERROR: parseTextBox - String was too long. Continuing with shortned string...");
    }else{
        returnedTextBox.text[i] = '\0'; // this is probably a very ghetto way of doing things;   
        //this is because while it may look like the string ends, there is still a bunch of junk data being thorwn around
    }

    if(input[select] == '?'){
        select++;
        if(input[select] == '('){
            select++;
        }
        i = 0;
        //printf("c: %c\n", input[select]);
        while(input[select] != ')' && i < 17){
            //printf("c: %c\n", input[select]);
            sendToParse[i] = input[select];
            select++;
            i++;
            
        }
        sendToParse[i] = ')';
        if(i == 17){
            printf("ERROR: parseTextBox - Color input was too long. Is input a valid color?");
        }
        printf("parseTextBox: cs: %s\n", sendToParse);
        returnedTextBox.colour = parseColor(sendToParse, i);
    }else{
        printf("ERROR: parseTextBox - Please input an item of type color");
    }

    return returnedTextBox;
}

//0=img value
//1=tag
//2=enabled
int parseWallTag(char input[128], int inputSize, int returnValue){
    int inputSelect = 0;
    char sendToParse[11];
    int i = 0;

    printf("parseWallTag: Reading \"%s\"\n", input);

    if(input[inputSelect] == '('){
        inputSelect++;
    }


    if(returnValue == 0){
        while(input[inputSelect] != ','){
            sendToParse[i] = input[inputSelect];
            i++;
            inputSelect++;
        }
        return parseInt(sendToParse, i);
    }else{
        while(input[inputSelect] != ','){
            inputSelect++;
        }
        inputSelect++;
        if(returnValue == 1){
            while(input[inputSelect] != ')'){
                sendToParse[i] = input[inputSelect];
                i++;
                inputSelect++;
            }
            return parseInt(sendToParse, i);
        }else if(returnValue == 2){
            while(input[inputSelect] != ','){
                inputSelect++;
            }
            inputSelect++;
            i = parseBool(input[inputSelect]);
            inputSelect++;
            if(input[inputSelect] == ')'){
                return i;
            }else{
                printf("WARNING: parseWallTag - Couldn't terminate property. Did you forget a closing bracket?\n");
            }
        }else{
            printf("ERROR: parseWallTag - Received incorrect returnValue. (Programming error) Skipping...\n");
            return 0;
        }
    }
    printf("ERROR: parseWallTag - Programming error. Why have we ended the function? Hacker Alert! Skipping...\n");
    return 0;
}

//SandwichFactory Data structure
//    ~ = property   = ~propertyname=data;
//    ; seperates each object (if on same line)
//    ! = terminates the file
//
//  Structs
//      symbol() or symbol{} or symbol) <- not an array
// @  % = level collider  = %()
// @  * = ladder collider = *()
// @  ^ = Physics object  = ^()
// *  < = triangle collider = <(p1x, p1y, p2x, p2y, aPx, aPy)
//    # = camera     = #(@(pos),@(maxCam),@(minCam),smoothingEnabled, smoothing);
//        note: can have either vec2 int or float
// @  & = textBox       = &{(text1,x,y,colour,size);(text2,x,y,colour,size);etc..}; or&()
//
//  Types
//    @ = vector 2 (int)   = @(x,y);  or @2091,133)
//    @f= vector 2 (float) = @f(x,y); or @f103.198,103.123)
//    ? = colour     = ?(r,g,b,a)
// *  " = string     = "(text) or "text) or "text"

// * = Not finished
// @ = Arrays not implemented

//    example: ~10;~224!
//    example: ~193;~1093;%{(13,31,3,13,34);(1093,13,42,24,24)};*{}

// Structs could be written without inital bracket:   #109,10193,1334,221) instead of #(13013,13,13,345,6,6)
// Vector2 is the same, @219,138) instead of @(1983,13)

//Bools can be given as 0, 1, T, F, t, f

// Note: Structs must be strictly written keeping their structure in mind, however an object can be written at any point

//Reads generic sandwichfactory file type. Returns error code if neededs
//
//int levelTexts, int levelCol, int ladderNum, int objectCol, bool disableCam, Vector2 startingPos
//add these values as you add more features

// ; TERMINATE CURRENT LOOP IF REACHED. DO NOT MAKE THE SEMICOLON MOVE THE OBJECT AS THE LOOP SHOULD BE DOING IT

// Property names
// ~sp=     (startingPosition)
// ~sp2=    (startingPosition2)
// ~img= (levelImage)
// ~leverNum=
// ~doorNum=
// ~isMultiplayer=
// ~goal=   (boxcollider2d)

//Arrays?
//%{13,31,3,13,34;1093,13,42,24,24}


int readFileSF(char path[128], 
            bool readLevelImagePath, char levelImagePath[64], 
            //bool* disableCam, 
            s_Camera* camera, 
            Vector2* startingPos, Vector2* startingPos2,
            BoxCollider2D levelCol[64], BoxCollider2D ladders[16], TextBox texts[2], PhysicsObject physobjects[8], Triangle triCol[10],
            int* levelTexts, int* levelColNum, int* ladderNum, int* physObjNum,
            int* isLever, int* isDoor, int* isMultiplayer, int* portalNum,
            int wallTags[16], int* wallNum, uint16_t* wallEnabled,
            BoxCollider2D* goal, int* scrollType
        ){
    
    
    FILE* fp;
    fp = fopen(path, "r");
    if(fp == NULL){
        printf("ERROR: readFileSF - ERROR opening. File may not exist.");
        printf("Could not open: %s\n", path);
        return 1;
    }
    
    char input[1024]; // Defines how much text is stored (ie. how big a level can be)
    char sendToParse[128];
    char propertyName[8];
    char ch; // character selected
    int levelColID = 0;
    int ladderID = 0;
    int physObjID = 0;
    int textBoxID = 0;
    //int triColID = 0;
    bool isEnd = false; // do we end read?
    bool isVector = false;
    int temp = 0;
    bool subFor = false;

    //*disableCam = true;
    int leverNum = 0;
    int doorNum = 0;
    *portalNum = 0;
    *wallNum = 0;
    *isMultiplayer = 0; // default
    *wallEnabled = 0x0000;

    bool getNewLine = false;

    //Note: This means only 1024 characters can be stored per line
    printf("FILE_READLINE_SF:");
    for(int i = 0; i < 1024 && !isEnd; i++){
        input[i] = fgetc(fp);
        if(input[i] == '\n' || input[i] == EOF){
            isEnd = true;
        }
        if(input[i] == '\n'){
            printf("\\n`\n");
        }else{
            printf("%c`", input[i]);
        }
    }
    isEnd = false;
    
    for(int charSelect = 0; charSelect < 1024 && !isEnd; charSelect++){
        if(subFor){
            subFor = false;
            charSelect--;
        }
        ch = input[charSelect];
        if(ch == '\n'){
            printf("\\n  \n");
        }else{
            printf("%c` ", ch);
        }
        
        if(ch == '!' || ch == '\0' || ch == EOF){
            isEnd = true;
        }else if(ch == '~'){
            charSelect++;
            ch = input[charSelect];
            for(int i = 0; ch != '=' && i < 32 && ch != '\n' && ch != '\0'; i++){
                propertyName[i] = ch;
                charSelect++;
                ch = input[charSelect];
                if(ch == '='){
                    propertyName[i + 1] = ';';
                }
            }
            if(ch != '='){
                printf("ERROR: parseProperty - Property name was too long or invald\n");
                printf("Given name: %s\n", propertyName);
                printf("ch = %c\n", ch);
            }else{
                charSelect++;
                ch = input[charSelect];
                if(strEquals(propertyName, "sp;")){
                    if(ch == '@'){
                        charSelect++;
                        ch = input[charSelect];
                        if(ch == 'f'){
                            //float vector
                            charSelect++;
                            ch = input[charSelect];
                            temp = 0;
                            while(ch != ')' && temp < 20){
                                sendToParse[temp] = ch;
                                charSelect++;
                                ch = input[charSelect];
                                temp++;
                            }
                            if(temp == 20){
                                printf("ERROR: parseProperty - Reached end of startingPos before expected\n");
                            }else{
                                sendToParse[temp] = ')';
                                *startingPos = readVector2Float(sendToParse);
                            }
                        }else{
                            //int vector
                            temp = 0;
                            while(ch != ')' && temp < 20){
                                sendToParse[temp] = ch;
                                charSelect++;
                                ch = input[charSelect];
                                temp++;
                            }
                            if(temp == 20){
                                printf("ERROR: parseProperty - Reached end of startingPos before expected\n");
                            }else{
                                sendToParse[temp] = ')';
                                *startingPos = readVector2(sendToParse);
                            }
                        }
                    }else{
                        printf("ERROR: parseProperty - Please provide a vector2 for the sp input\n");
                    }
                }else if(strEquals(propertyName, "sp2;")){
                    if(ch == '@'){
                        charSelect++;
                        ch = input[charSelect];
                        if(ch == 'f'){
                            //float vector
                            charSelect++;
                            ch = input[charSelect];
                            temp = 0;
                            while(ch != ')' && temp < 20){
                                sendToParse[temp] = ch;
                                charSelect++;
                                ch = input[charSelect];
                                temp++;
                            }
                            if(temp == 20){
                                printf("ERROR: parseProperty - Reached end of startingPos before expected\n");
                            }else{
                                sendToParse[temp] = ')';
                                *startingPos2 = readVector2Float(sendToParse);
                            }
                        }else{
                            //int vector
                            temp = 0;
                            while(ch != ')' && temp < 20){
                                sendToParse[temp] = ch;
                                charSelect++;
                                ch = input[charSelect];
                                temp++;
                            }
                            if(temp == 20){
                                printf("ERROR: parseProperty - Reached end of startingPos before expected\n");
                            }else{
                                sendToParse[temp] = ')';
                                *startingPos2 = readVector2(sendToParse);
                            }
                        }
                    }else{
                        printf("ERROR: parseProperty - Please provide a vector2 for the sp input\n");
                    }
                }else if(strEquals(propertyName, "img;")){
                    temp = 0;
                    //Note(BUG): if we read the EOF, the property doesnt terminate properly resulting in garbage data
                    while(ch != ';' && ch != '\n' && ch != '\0' && temp < 64 && ch != EOF){
                        levelImagePath[temp] = ch;
                        charSelect++;
                        ch = input[charSelect];
                        temp++;
                    }
                    if(temp == 64){
                        //printf("ERROR: Overflowed allow path size (64 charcters). File missing, forgot to terminate or try renaming the file.\n");
                        printf("ERROR: parseProperty - Overflowed allowed path size (64 charcters). Forgot to terminate or try renaming the file.\n");
                    }else{
                        levelImagePath[temp] = '\0';
                        printf("parseProperty: Level Image set to %s\n", levelImagePath);
                    }
                    charSelect--;
                }else if(strEquals(propertyName, "leverNum;")){
                    temp = 0;
                    while(ch != ';' && ch != '\n' && ch != '\0' && temp < 12 && ch != EOF){
                        sendToParse[temp] = ch;
                        charSelect++;
                        ch = input[charSelect];
                        temp++;
                    }
                    sendToParse[temp] = '\0';
                    leverNum = parseInt(sendToParse, temp);
                }else if(strEquals(propertyName, "doorNum;")){
                    temp = 0;
                    while(ch != ';' && ch != '\n' && ch != '\0' && temp < 12 && ch != EOF){
                        sendToParse[temp] = ch;
                        charSelect++;
                        ch = input[charSelect];
                        temp++;
                    }
                    sendToParse[temp] = '\0';
                    doorNum = parseInt(sendToParse, temp);
                }else if(strEquals(propertyName, "wallNum;")){
                    temp = 0;
                    while(ch != ';' && ch != '\n' && ch != '\0' && temp < 12 && ch != EOF){
                        sendToParse[temp] = ch;
                        charSelect++;
                        ch = input[charSelect];
                        temp++;
                    }
                    sendToParse[temp] = '\0';
                    *wallNum = parseInt(sendToParse, temp);
                }else if(strEquals(propertyName, "portalNum;")){
                    temp = 0;
                    while(ch != ';' && ch != '\n' && ch != '\0' && temp < 12 && ch != EOF){
                        sendToParse[temp] = ch;
                        charSelect++;
                        ch = input[charSelect];
                        temp++;
                    }
                    sendToParse[temp] = '\0';
                    *portalNum = parseInt(sendToParse, temp);
                }else if(strEquals(propertyName, "playerImg")){
                    printf("ERROR: parseProperty: playerImg not implemented yet. Skipping...\n");
                }else if(strEquals(propertyName, "isMultiplayer;")){
                    *isMultiplayer = parseBool(ch);
                    charSelect++;
                    ch = input[charSelect];
                }else if(strEquals(propertyName, "goal;")){
                    if(ch == '%'){
                        charSelect++;
                        ch = input[charSelect];
                        
                        if(ch == '('){
                            charSelect++;
                            ch = input[charSelect];
                        }
                        temp = 0;
                        while(ch != ')' && ch != '\n' && ch != '\0' && ch != EOF){
                            sendToParse[temp] = ch;
                            charSelect++;
                            ch = input[charSelect];
                            temp++;
                        }
                        sendToParse[temp] = ch;
                        *goal = parseBoxCollider(sendToParse, temp, false);
                    }else{
                        printf("ERROR: parseProperty - Please provide a BoxCollider2D for the goal input\n");
                    }
                }else if(strEquals(propertyName, "scrollType;")){
                    temp = 0;
                    while(ch != ';' && ch != '\n' && ch != '\0' && temp < 12 && ch != EOF){
                        sendToParse[temp] = ch;
                        charSelect++;
                        ch = input[charSelect];
                        temp++;
                    }
                    sendToParse[temp] = '\0';
                    *scrollType = parseInt(sendToParse, temp);
                }else{
                    printf("ERROR: parseProperty - Invalid property name\n");
                    printf("Given name: %s\n", propertyName);
                }

                
                if(ch != ';' && ch != '!'&& ch != '\n' && ch != '\0'){
                    printf("ERROR: reafFileSF - Error terminating Property");
                }else{
                    charSelect--;
                }
            }

            
        }else if(ch == '#'){
            printf("readFileSF: attempting to parseCamera\n");
            printf("WARNING: readFileSF - Camera reading disabled for this release. Skipping line...");
            getNewLine = true;
            /*s_Camera temp_cam;
            charSelect++;
            ch = input[charSelect];
            if(ch == '('){
                charSelect++;
                ch = input[charSelect];
            }
            
            for(int s = 0; s < 3; ){
                if(ch == '@'){
                    charSelect++;
                    ch = input[charSelect];
                    if(ch == 'f'){
                        temp = 1;
                        charSelect++;
                        ch = input[charSelect];
                    }else{
                        temp = 0;   
                    }
                    for(int i = 0; ch != ')'; i++){
                        sendToParse[i] = ch;
                        charSelect++;
                        ch = input[charSelect];
                        if(ch == ')'){
                            sendToParse[i + 1] = ch;
                        }
                    }
                    if(temp == 1){
                        switch(s){
                            case 0:
                                temp_cam.position = readVector2Float(sendToParse);
                                break;
                            case 1:
                                temp_cam.maxCamera = readVector2Float(sendToParse);
                                break;
                            case 2:
                                temp_cam.minCamera = readVector2Float(sendToParse);
                                break;
                        }
                    }else{
                        switch(s){
                            case 0:
                                temp_cam.position = readVector2(sendToParse);
                                break;
                            case 1:
                                temp_cam.maxCamera = readVector2(sendToParse);
                                break;
                            case 2:
                                temp_cam.minCamera = readVector2(sendToParse);
                                break;
                        }
                    }
                    charSelect++;
                    ch = input[charSelect];
                    printf("-%c-", ch);
                    /disableCam = false;
                }else if(ch == ','){
                    s++;
                    charSelect++;
                    ch = input[charSelect];
                }else {
                    printf("ERROR: readFileSF - Vector2 - I'm too lazy to do the x/y method please use vector2 import\n");
                }
            }

            if(ch == ','){
                temp_cam.smoothingEnabled = false;
                charSelect++;
                ch = input[charSelect];
            }else{
                temp_cam.smoothingEnabled = parseBool(ch);
                charSelect++;
                ch = input[charSelect];
            }

            if(ch == ','){
                charSelect++;
                ch = input[charSelect];
            }else{
                printf("ERROR: readFileSF - Camera - Value given for smoothingEnabled is not bool");
            }

            for(int i = 0; ch != ')' && ch != ';'; i++){
                sendToParse[i] = ch;
                charSelect++;
                ch = input[charSelect];
            }
            temp_cam.smoothing = parseFloat(sendToParse);
            *camera = temp_cam;*/
        }else if(ch == '%'){
            //level collider  = %{} or %()
            charSelect++;
            ch = input[charSelect];
            if(ch == '{'){
                //array
                while(ch != '}'){
                    charSelect++;
                    ch = input[charSelect];
                }
                
            }else{
                //single struct
                if(ch == '('){
                    charSelect++;
                    ch = input[charSelect];
                }
                temp = 0;
                while(ch != ')'){
                    sendToParse[temp] = ch;
                    charSelect++;
                    ch = input[charSelect];
                    temp++;
                }
                sendToParse[temp] = ch;
                levelCol[levelColID] = parseBoxCollider(sendToParse, temp, false);
                //printf("uhh this worked");
                printf("parseBoxCollider (main): currentId = %d\n", levelColID);
                levelColID++;
            }
        }else if(ch == '*'){
            //ladder collider = *{} or *()
            charSelect++;
            ch = input[charSelect];
            if(ch == '{'){
                //array
            }else{
                //single struct
                if(ch == '('){
                    charSelect++;
                    ch = input[charSelect];
                }
                temp = 0;
                while(ch != ')'){
                    sendToParse[temp] = ch;
                    charSelect++;
                    ch = input[charSelect];
                    temp++;
                }
                sendToParse[temp] = ch;
                ladders[ladderID] = parseBoxCollider(sendToParse, temp, false); // fix this to be based on input
                ladders[ladderID].ladder = true;
                printf("parseBoxCollider (main - ladder): currentLadderId = %d\n", ladderID);
                ladderID++;
            }
        }else if(ch == '^'){
            //physics object  = ^{} or ^()
            printf("readFileSF: attempting to parsePhysObj\n");
            charSelect++;
            ch = input[charSelect];
            if(ch == '{'){
                //array
            }else{
                //single struct
                if(ch == '('){
                    charSelect++;
                    ch = input[charSelect];
                }
                temp = 0;
                isVector = false;
                while(ch != ')'){
                    if(ch == '@'){
                        isVector = true;
                    }
                    sendToParse[temp] = ch;
                    charSelect++;
                    ch = input[charSelect];
                    temp++;
                    if(ch == ')' && isVector){
                        isVector = false;
                        charSelect++;
                        ch = input[charSelect];
                        temp++;
                    }
                }
                sendToParse[temp] = ch;
                physobjects[physObjID] = parsePhysicsObject(sendToParse, temp);
                physObjID++;
            }
        }else if(ch == '$'){
            //wallTag  = $()
            printf("readFileSF: attempting to parseWallTag\n");
            charSelect++;
            ch = input[charSelect];
            if(ch == '{'){
                //array
            }else{
                //single struct
                if(ch == '('){
                    charSelect++;
                    ch = input[charSelect];
                }
                temp = 0;
                while(ch != ')'){
                    sendToParse[temp] = ch;
                    charSelect++;
                    ch = input[charSelect];
                    temp++;
                }
                sendToParse[temp] = ch;
                //(*wallTags)[parseWallTag(sendToParse, temp, false)] = parseWallTag(sendToParse, temp, true);
                wallTags[parseWallTag(sendToParse, temp, 0)] = parseWallTag(sendToParse, temp, 1);
                *wallEnabled = *wallEnabled | (parseWallTag(sendToParse, temp, 2) << parseWallTag(sendToParse, temp, 0));
            }
        }else if(ch == '&'){
            //textBox         = &{} or &()
            charSelect++;
            ch = input[charSelect];
            if(ch == '{'){
                //array
            }else{
                //single struct
                if(ch == '('){
                    charSelect++;
                    ch = input[charSelect];
                }
                temp = 0;
                isVector = false;
                while(ch != ')'){
                    if(ch == '?'){
                        isVector = true;
                    }
                    sendToParse[temp] = ch;
                    charSelect++;
                    ch = input[charSelect];
                    temp++;
                    if(ch == ')' && isVector){
                        isVector = false;
                        charSelect++;
                        ch = input[charSelect];
                        temp++;
                    }
                }
                sendToParse[temp] = ch;
                texts[textBoxID] = parseTextBox(sendToParse, temp);
                printf("readFileSF: TextBox - Given text: %s\n", texts[textBoxID].text);
                //printf("hello3");
                textBoxID++;
            }
        }else if(ch == '<'){
            //physics object  = ^{} or ^()
            //triangle
            printf("readFileSF: attempting to parseTriangle\n");
            printf("WARNING: readFileSF - Triangle reading disabled for this release. Skipping line...");
            getNewLine = true;
            /*charSelect++;
            ch = input[charSelect];
            if(ch == '{'){
                //array
            }else{
                //single struct
                if(ch == '('){
                    charSelect++;
                    ch = input[charSelect];
                }
                temp = 0;
                isVector = false;
                while(ch != ')'){
                    if(ch == '@'){
                        isVector = true;
                    }
                    sendToParse[temp] = ch;
                    charSelect++;
                    ch = input[charSelect];
                    temp++;
                    if(ch == ')' && isVector){
                        isVector = false;
                        charSelect++;
                        ch = input[charSelect];
                        temp++;
                    }
                }
                sendToParse[temp] = ch;
                triCol[triColID] = parseTriangle(sendToParse, temp);
                triColID++;
            }*/
        }else if(ch == '\n' || ch == '/'){
            getNewLine = true;
        }else{
            if(ch != ' ') printf("WARNING: readFileSF - Received unexpected character: %c\n", ch);
        }

        if(getNewLine){
            charSelect = 0;
            printf("FILE_READLINE_SF:");
            for(int i = 0; i < 1024 && !isEnd; i++){
                input[i] = fgetc(fp);
                if(input[i] == '\n' || input[i] == EOF){
                    isEnd = true;
                    subFor = true; // Because we always do charSelect at the end, we must subtract that value to return to 0 instead of 1
                }
                if(input[i] == '\n'){
                    printf("\\n`\n");
                }else{
                    printf("%c`", input[i]);
                }
            }
            isEnd = false;
            printf("    Getting new line\n");
            getNewLine = false;
        }
    }
    levelColID -= (doorNum + leverNum + *portalNum);
    *isDoor = doorNum;
    *isLever = leverNum;

    *levelColNum = levelColID;
    *levelTexts = textBoxID;
    *ladderNum = ladderID;
    *physObjNum = physObjID;

    //printf("input - %s", input);
    printf("readFileSF: crate1: %f, %f, %d, %d, %d, %d\n", physobjects[0].position.x, physobjects[0].position.y, physobjects[0].sizeX, physobjects[0].sizeY, physobjects[0].trigger, physobjects[0].enabled);
    printf("readFileSF: crate2: %f, %f, %d, %d, %d, %d\n", physobjects[1].position.x, physobjects[1].position.y, physobjects[1].sizeX, physobjects[1].sizeY, physobjects[1].trigger, physobjects[1].enabled);
    printf("readFileSF: levelColIDs = %d ; doorNum = %d ; leverNum = %d ; ladderNum = %d ; crateNum = %d\n", levelColID, doorNum, leverNum, ladderID, physObjID);
    printf("readFileSF: ladder1: %d, %d, %d, %d, %d, %d, %d\n", ladders[0].x, ladders[0].y, ladders[0].sizeX, ladders[0].sizeY, ladders[0].trigger, ladders[0].ladder, ladders[0].enabled);
    printf("readFileSF: portal1: %d, %d, %d, %d, %d, %d, %d\n", levelCol[levelColID + doorNum + leverNum].x, levelCol[levelColID + doorNum + leverNum].y, levelCol[levelColID + doorNum + leverNum].sizeX, levelCol[levelColID + doorNum + leverNum].sizeY, levelCol[levelColID + doorNum + leverNum].trigger, levelCol[levelColID + doorNum + leverNum].ladder, levelCol[levelColID + doorNum + leverNum].enabled);
    //printf("crate1(game): %f, %f, %d, %d, %d, %d\n", crate[0].position.x, crate[0].position.y, crate[0].sizeX, crate[0].sizeY, crate[0].trigger, crate[0].enabled);
    //printf("crate2(game): %f, %f, %d, %d, %d, %d\n", crate[1].position.x, crate[1].position.y, crate[1].sizeX, crate[1].sizeY, crate[1].trigger, crate[1].enabled);
    
    fclose(fp);
    //printf("hello5\n");
    return 0;
}





// Reads generic int data 
//   int;int;int!
void readFileInt(char path[40], int inputArray[256], int arraySize, int* outputSize){
    FILE* fp;
    fp = fopen(path, "r");
    if(fp == NULL){
        printf("ERROR: ERROR opening");
        return;
    }

    char input[512];
    char ch;
    int inputSelect = 0;

    fgets(input, 512, fp);

    bool isEnd = false;

    for(int i = 0; i < 511 && !isEnd && inputSelect < arraySize; i++){
        ch = input[i];

        switch(ch){
            case '!':
                isEnd = true;
                break;
            case '0':
                inputArray[inputSelect] = inputArray[inputSelect] * 10;
                break;
            case '1':
                inputArray[inputSelect] = (inputArray[inputSelect] * 10) + 1;
                break;
            case '2':
                inputArray[inputSelect] = (inputArray[inputSelect] * 10) + 2;
                break;
            case '3':
                inputArray[inputSelect] = (inputArray[inputSelect] * 10) + 3;
                break;
            case '4':
                inputArray[inputSelect] = (inputArray[inputSelect] * 10) + 4;
                break;
            case '5':
                inputArray[inputSelect] = (inputArray[inputSelect] * 10) + 5;
                break;
            case '6':
                inputArray[inputSelect] = (inputArray[inputSelect] * 10) + 6;
                break;
            case '7':
                inputArray[inputSelect] = (inputArray[inputSelect] * 10) + 7;
                break;
            case '8':
                inputArray[inputSelect] = (inputArray[inputSelect] * 10) + 8;
                break;
            case '9':
                inputArray[inputSelect] = (inputArray[inputSelect] * 10) + 9;
                break;
            case ';':
                inputSelect++;
                break;
        }
    }
    fclose(fp);
    if(!isEnd){
        printf("ERROR: Reached end of file before file termination");
    }else{
        *outputSize = inputSelect;
    }
    for(int i = 0; i < arraySize; i++){
        printf("%d\n", inputArray[i]);
    }
}


//Writes int array
void writeFileInt(char path[40], int inputArray[256], int arraySize){
    FILE* fp;
    fp = fopen(path, "w");

    char str[40];

    for(int i = 0; i < arraySize; i++){
        sprintf(str, "%d", inputArray[i]);
        fputs(str, fp);
        fputc(';', fp);
    }
    fputc('!', fp);
    fclose(fp);
}