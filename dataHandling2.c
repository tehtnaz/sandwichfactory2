#include "dataHandling2.h"

typedef enum TOKEN_TYPE{
    //void + error types
    UNDEFINED,
    NO_TYPE,

    // --STRUCTS-- auto allocated   //| Argument count
    PROPERTY,                       //| 2
    WALL_TAG,                       //| 3
    LEVEL_COL,                      //| 4
    PHYS_OBJ, TEXT_BOX,             //| 5
    LADDER_COL, DOOR, PORTAL, WALL, //| 6
    LEVER,                          //| 7

    // --TYPES-- must be associated to a PROPERTY or STRUCT
    //complex types
        VECTOR_2, // 2
        COLOUR,   // 3
    //basic types
        TRUE, FALSE,
        INTEGER, FLOAT,
        STRING,

    //modifier
        MINUS,

    //groups
    LEFT_PAREN,
    RIGHT_PAREN

}TOKEN_TYPE;

typedef struct TokenInfo{
    union{
        char* text;
        int integer;
        float decimal;
    };
    TOKEN_TYPE type;
    int line; 
    struct TokenInfo* next;
}TokenInfo;

/*typedef enum GROUP_TYPE{
    STRUCT,
    COMPLEX_TYPE,
    BASIC_TYPE
}GROUP_TYPE;*/

typedef struct StructGroup{
    TokenInfo token;
    struct StructGroup* child;
    struct StructGroup* parent;
    struct StructGroup* next;
    struct StructGroup* previous;
}StructGroup;

// --- Helpers ---

int parseInt(const char* input){
    return TextToInteger(input);
}

float parseFloat(const char* input){
    return (float)atof(input);
}

bool isDigit(char ch){
    return ch >= '0' && ch <= '9';
}
bool isAlpha(char ch){
    return (ch >= 'a' && ch <= 'z') || 
           (ch >= 'A' && ch <= 'Z') ||
           (ch == '_');
}
bool isAlphaNumeric(char ch){
    return isAlpha(ch) || isDigit(ch);
}

// --- Scanning ---

static char ch = '\0'; // character selected
static char peek = '\0'; // next character selected
static FILE* fp = NULL;
static int line = 1;
static TokenInfo* tokenInfo = NULL;

void advanceChar(){
    ch = peek;
    if(peek != EOF)
        peek = fgetc(fp);
}

void readString(TokenInfo* tokenInfo){
    tokenInfo->type = STRING;
    int stringSize = 8;
    tokenInfo->text = (char*)malloc(sizeof(char) * stringSize);
    for(int i = 0; peek != '"'; i++){
        if(i == (stringSize - 1)){
            stringSize *= 2;
            tokenInfo->text = (char*)realloc(tokenInfo->text, sizeof(char) * stringSize);
        }
        advanceChar();
        tokenInfo->text[i] = ch;
        if(peek == '"' || peek == EOF){
            i++;
            tokenInfo->text[i] = '\0';
            if(peek != EOF){
                //advanceChar();
            }else{
                printf("WARNING: readFileSF[scan/readString] - Unterminated string at end of file\n");
                break;
            }
        }
        if(peek == '\\'){
            advanceChar();
            advanceChar();
            i++;
            switch(ch){
                case 'n':  tokenInfo->text[i] = '\n'; break;
                case 't':  tokenInfo->text[i] = '\t'; break;
                case '\\': tokenInfo->text[i] = '\\'; break;
                case '"':  tokenInfo->text[i] = '"';  break;
                default:   tokenInfo->text[i] = '\\'; break;
            }
        }
    }
    advanceChar();
}

void readNumber(TokenInfo* tokenInfo){
    char* string;
    int stringSize = 8;
    tokenInfo->type = INTEGER;
    string = (char*)malloc(sizeof(char) * stringSize);
    string[0] = ch;
    for(int i = 1; isDigit(peek); i++){
        if(i == stringSize){
            stringSize *= 2;
            string = (char*)realloc(string, sizeof(char) * stringSize);
        }
        advanceChar();
        string[i] = ch;
        if(peek == '.'){
            advanceChar();
            tokenInfo->type = FLOAT;
        }
    }
    if(tokenInfo->type == INTEGER){
        tokenInfo->integer = parseInt(string);
    }else{
        tokenInfo->decimal = parseFloat(string);
    }
    free(string);
}
void readKeyword(TokenInfo* tokenInfo){
    tokenInfo->type = UNDEFINED;
    int stringSize = 8;
    tokenInfo->text = (char*)malloc(sizeof(char) * stringSize);
    tokenInfo->text[0] = ch;
    int i = 1;
    while(isAlphaNumeric(peek)){
        if(i == (stringSize - 1)){
            stringSize *= 2;
            tokenInfo->text = (char*)realloc(tokenInfo->text, sizeof(char) * stringSize);
        }
        advanceChar();
        tokenInfo->text[i] = ch;
        i++;
    }
    tokenInfo->text[i] = '\0';
    printf("[DEBUG] - readKeyword - %s\n", tokenInfo->text);
    if(TextIsEqual(tokenInfo->text, "true")){
        tokenInfo->type = TRUE;
    }else if(TextIsEqual(tokenInfo->text, "false")){
        tokenInfo->type = FALSE;
    }else if(TextIsEqual(tokenInfo->text, "Lever")){
        tokenInfo->type = LEVER;
    }else if(TextIsEqual(tokenInfo->text, "Door")){
        tokenInfo->type = DOOR;
    }else if(TextIsEqual(tokenInfo->text, "Portal")){
        tokenInfo->type = PORTAL;
    }else if(TextIsEqual(tokenInfo->text, "Wall")){
        tokenInfo->type = WALL;
    }else{
        printf("WARNING: readFileSF[scan/readKeyword] - [line %d] Item doesn't match any keywords\n", line);
    }
    free(tokenInfo->text);
    tokenInfo->text = NULL;
}

// --- Grouping --- 
void advanceToken(){
    tokenInfo = tokenInfo->next;
}

StructGroup* sg_alloc(){
    return (StructGroup*)calloc(1, sizeof(StructGroup));
}
// push struct group to struct group linked list
void pushGroup(StructGroup* group, StructGroup* item){
    if(group == NULL){
        printf("[DEBUG] WARNING: dataHandling2[readFileSF/group/addGroup] - Attempted adding to NULL group. Skipping...");
    }
    if(group->next == NULL){
        group->next = item;
    }
    StructGroup* temp;
    temp = group->next;
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = item;
}


// --- Parsing --- 

//does not include grandchildren, only direct children
int childNum(StructGroup* group){
    //Fucntion panics
        if(group == NULL) return -1;

    StructGroup* item = group->child;
    int i = 0;
    while(item != NULL){
        item = item->next;
        i++;
    }
    return i;
}

//check if a group has enough args to be satisfied
void checkArgNumber(StructGroup* group, int expectedArgs, const char* itemName){
    if(childNum(group) != expectedArgs) printf("WARNING: readFileSF[parse/checkArgNumber] - [line %d] Unexpected number of arguments given to %s\n", group->token.line, itemName);
}

//types
Vector2 parseVector2(StructGroup* group){
    checkArgNumber(group, 2, "Vector2");
    Vector2 vector2 = {0, 0};
    StructGroup* temp = group->child;
    for(int i = 0; i < 2 && temp != NULL; i++){
        if(temp->token.type == INTEGER){
            if(i == 0) vector2.x = temp->token.integer;
            else vector2.y = temp->token.integer;
        }else if(temp->token.type == FLOAT){
            if(i == 0) vector2.x = temp->token.decimal;
            else vector2.y = temp->token.decimal;
        }else{
            printf("WARNING: readFileSF[parse/parseVector2] - [line %d] Received non-number argument. Argument skipped.\n", temp->token.line);
        }
        temp = temp->next;
    }
    return vector2;
}
Color parseColor(StructGroup* group){
    checkArgNumber(group, 4, "Color");
    Color color = {0, 0, 0, 0};
    StructGroup* temp = group->child;
    for(int i = 0; i < 4 && temp != NULL; i++){
        if(temp->token.type == INTEGER){
            int limited = temp->token.integer;
            if(limited > 255 || limited < 0){
                printf("WARNING: readFileSF[parse/parseColor] - [line %d] Received integer that was either not within range [0-255]. Argument has been snapped to closest valid value..\n", temp->token.line);
                if(limited > 255) limited = 255;
                else limited = 0;
            }
            if(i == 0) color.r = limited;
            else if(i == 1) color.g = limited;
            else if(i == 2) color.b = limited;
            else color.a = limited;
        }else{
            printf("WARNING: readFileSF[parse/parseColor] - [line %d] Received non-integer argument. Argument skipped.\n", temp->token.line);
        }
        temp = temp->next;
    }
    return color;
}
//structs
BoxCollider2D parseBoxCollider2D(StructGroup* group){
    BoxCollider2D box = {0, 0, 0, 0, 0, false, true, 0};
    StructGroup* temp = group->child;
    if(group->token.type == LADDER_COL){
        box.ladder = true;
    }
    for(int i = 0; i < 4 && temp != NULL; i++){
        if(temp->token.type == INTEGER){
            if(i == 0) box.x = temp->token.integer;
            else if(i == 1) box.y = temp->token.integer;
            else if(i == 2) box.sizeX = temp->token.integer;
            else box.sizeY = temp->token.integer;
        }else{
            printf("WARNING: readFileSF[parse/parseBoxCollider2D] - [line %d] Received non-integer argument [arg %d]. Argument skipped.\n", temp->token.line, i + 1);
        }
        temp = temp->next;
    }
    //trigger, tag, enabled
    if(!(group->token.type == LEVEL_COL)){
        if(temp == NULL){
            printf("WARNING: readFileSF[parse/parseBoxCollider2D] - [line %d] Not enough arguments provided\n", group->token.line);
            return box;
        }
        if(temp->token.type != INTEGER){
            printf("WARNING: readFileSF[parse/parseBoxCollider2D] - [line %d] Received non-integer argument for arg 5. Argument skipped.\n", temp->token.line);
        }else if(temp->token.integer > 0){
            if(group->token.type == LEVER || group->token.type == PORTAL){
                box.trigger = temp->token.integer;
            }else{
                box.tag = temp->token.integer;
            }
        }else{
            printf("WARNING: readFileSF[parse/parseBoxCollider2D] - [line %d] Received negative integer argument for arg 5. Argument skipped.\n", temp->token.line);
        }
        temp = temp->next;
        if(temp == NULL){
            printf("WARNING: readFileSF[parse/parseBoxCollider2D] - [line %d] Not enough arguments provided\n", group->token.line);
            return box;
        }
        if(temp->token.type != TRUE && temp->token.type != FALSE){
            printf("WARNING: readFileSF[parse/parseBoxCollider2D] - [line %d] Received non-boolean argument for arg 6 [enabled]. Argument skipped.\n", temp->token.line);
        }else{
            box.enabled = temp->token.type == TRUE;
        }
        if(group->token.type == LEVER){
            temp = temp->next;
            if(temp == NULL){
                printf("WARNING: readFileSF[parse/parseBoxCollider2D] - [line %d] Not enough arguments provided\n", group->token.line);
                return box;
            }
            if(temp->token.type != INTEGER){
                printf("WARNING: readFileSF[parse/parseBoxCollider2D] - [line %d] Received non-integer argument for arg 7 [trigger]. Argument skipped.\n", temp->token.line);
            }else if(temp->token.integer > 0){
                box.tag = temp->token.integer;
            }
        }
    }else{
        checkArgNumber(group, 4, "Level Collider");
    }
    return box;
}
PhysicsObject parsePhysicsObject(StructGroup* group){
    checkArgNumber(group, 5, "PhysicsObject");
    StructGroup* temp = group->child;
    PhysicsObject physObj = {{0, 0}, 0, 0, {0, 0}, false, false, false};
    if(temp != NULL){
        if(temp->token.type == VECTOR_2){
            physObj.position = parseVector2(temp);
        }else{
            printf("WARNING: readFileSF[parse/parsePhysicsObject] - [line %d] Received non-vector2 argument. Argument skipped.\n", temp->token.line);
        }
        temp = temp->next;
        for(int i = 0; i < 2 && temp != NULL; i++){
            if(temp->token.type == INTEGER){
                if(i == 0) physObj.sizeX = temp->token.integer;
                else physObj.sizeY = temp->token.integer;
            }else{
                printf("WARNING: readFileSF[parse/parsePhysicsObject] - [line %d] Received non-integer argument. Argument skipped.\n", temp->token.line);
            }
            temp = temp->next;
        }
        for(int i = 0; i < 2 && temp != NULL; i++){
            if(temp->token.type == TRUE || temp->token.type == FALSE){
                if(i == 0) physObj.trigger = temp->token.type == TRUE;
                else physObj.enabled = temp->token.type == TRUE;
            }else{
                printf("WARNING: readFileSF[parse/parsePhysicsObject] - [line %d] Received non-boolean argument. Argument skipped.\n", temp->token.line);
            }
            temp = temp->next;
        }
    }
    return physObj;
}
TextBox parseTextBox(StructGroup* group){
    checkArgNumber(group, 5, "TextBox");
    StructGroup* temp = group->child;
    TextBox textBox = {0, 0, 0, "", {0, 0, 0, 0}};
    for(int i = 0; i < 3 && temp != NULL; i++){
        if(temp->token.type == INTEGER){
            if(i == 0) textBox.x = temp->token.integer;
            else if(i == 1) textBox.y = temp->token.integer;
            else textBox.size = temp->token.integer;
        }else{
            printf("WARNING: readFileSF[parse/parseTextBox] - [line %d] Received non-integer argument. Argument skipped.\n", temp->token.line);
        }
        temp = temp->next;
    }
    if(temp != NULL){
        if(temp->token.type == STRING){
            if(TextLength(temp->token.text) > 39){
                printf("WARNING: readFileSF[parse/parseTextBox] - [line %d] TextBox text cannot be more than 39 characters. String has been trimmed to fit\n", temp->token.line);
                TextCopy(textBox.text, TextSubtext(temp->token.text, 0, 39));
            }else{
                TextCopy(textBox.text, temp->token.text);
            }
        }else{
            printf("WARNING: readFileSF[parse/parseTextBox] - [line %d] Received non-string argument. Argument skipped.\n", temp->token.line);
        }
        temp = temp->next;
    }
    if(temp != NULL){
        if(temp->token.type == COLOUR){
            textBox.colour = parseColor(temp);
        }else{
            printf("WARNING: readFileSF[parse/parseTextBox] - [line %d] Received non-colour argument. Argument skipped.\n", temp->token.line);
        }
    }
    return textBox;
}
typedef struct WallTag{
    int wallId;
    int tag;
    bool enabled;
}WallTag;
WallTag parseWallTag(StructGroup* group){
    checkArgNumber(group, 3, "WallTag");
    WallTag wallTag = {0, 0, false};
    StructGroup* temp = group->child;
    for(int i = 0; i < 2 && temp != NULL; i++){
        if(temp->token.type == INTEGER){
            if(i == 0) wallTag.wallId = temp->token.integer;
            else wallTag.tag = temp->token.integer;
        }else{
            printf("WARNING: readFileSF[parse/parseWallTag] - [line %d] Received non-integer argument. Argument skipped.\n", temp->token.line);
        }
        temp = temp->next;
    }
    if(temp != NULL){
        if(temp->token.type == TRUE || temp->token.type == FALSE){
            wallTag.enabled = temp->token.type == TRUE;
        }else{
            printf("WARNING: readFileSF[parse/parseWallTag] - [line %d] Received non-boolean argument. Argument skipped.\n", temp->token.line);
        }
    }
    return wallTag;
}

uint64_t parseBoolArray64(StructGroup* group){
    StructGroup* temp = group->child;
    uint64_t output = 0;
    for(int i = 0; i < 64 && temp != NULL; i++){
        if(temp->token.type == TRUE || temp->token.type == FALSE){
            output |= ((uint64_t)(temp->token.type == TRUE)) << i;
        }else{
            printf("WARNING: readFileSF[parse/parseBoolArray64] - [line %d] Received non-boolean argument. Argument skipped.\n", temp->token.line);
        }
        temp = temp->next;
    }
    return output;
}

int readFileSF(char path[128], 
            char levelImagePath[64], char backgroundPath[64],
            Vector2* startingPos, Vector2* startingPos2,
            BoxCollider2D levelCol[64], BoxCollider2D ladders[16], TextBox texts[2], PhysicsObject physobjects[8],
            int* levelTexts, int* levelColNum, int* ladderNum, int* physObjNum,
            int* leverNum, int* doorNum, int* isMultiplayer, int* portalNum,
            int wallTags[16], int* wallImgNum, uint16_t* wallEnabled,
            BoxCollider2D* goal, int* scrollType, uint64_t* leverFlip)
{
    fp = fopen(path, "r");
    if(fp == NULL){
        printf("ERROR: readFileSF - ERROR opening. File may not exist.\n");
        printf("Could not open: %s\n", path);
        return 1;
    }
    ch = '\0'; // character selected
    peek = '\0'; // next character selected
    line = 1;
    tokenInfo = NULL;

    bool isEnd = false; // do we end read?

    // ----------------
    // readFileSF[scan]
    // ----------------
    printf("INFO: Initialize readFileSF[scan]\n");
    TokenInfo* infoRoot;
    infoRoot = (TokenInfo*)calloc(1, sizeof(TokenInfo));

    tokenInfo = infoRoot;
    tokenInfo->type = UNDEFINED;
    tokenInfo->next = NULL;
    peek = fgetc(fp);

    while(!isEnd){
        advanceChar();
        switch (ch){
            // structs / types
            case '%': tokenInfo->type = LEVEL_COL;  break;
            case '*': tokenInfo->type = LADDER_COL; break;
            case '^': tokenInfo->type = PHYS_OBJ;   break;
            case '&': tokenInfo->type = TEXT_BOX;   break;
            case '$': tokenInfo->type = WALL_TAG;   break;
            case '?': tokenInfo->type = COLOUR;     break;
            case '~': tokenInfo->type = PROPERTY;   break; // *now* it's a struct!
            //other 1 chars
            case '[': case '(': tokenInfo->type = LEFT_PAREN;     break;
            case ']': case ')': tokenInfo->type = RIGHT_PAREN;    break;

            case '-': tokenInfo->type = MINUS;    break;
            case ',': /* ignore */ break;
            
            case '@': 
                tokenInfo->type = VECTOR_2;
                if(peek == 'f'){
                    printf("WARNING: readFileSF[scan] - Vector2Float is deprecated, Vector2 now accepts floats\n");
                    advanceChar();
                }

            case ' ':
            case '\r':
            case '\t':
                // lol you're irrelevent, ignoring whitespace
                break;
            
            case '\n':
                line++;
                break;
            case EOF:
                isEnd = true;
                break;

            case '/':
                while(peek != EOF && peek != '\n') advanceChar();
                break;

            case '"':
                readString(tokenInfo);
                break;
            default:
                if(isDigit(ch)){
                    readNumber(tokenInfo);
                }else if(isAlphaNumeric(ch)){
                    readKeyword(tokenInfo);
                }else{
                    printf("WARNING: readFileSF[scan] - [line %d] Unexpected character: [%c]\n", line, ch);
                }
                break;
        }
        tokenInfo->line = line;

        if(!isEnd && tokenInfo->type != UNDEFINED){
            tokenInfo->next = (TokenInfo*)calloc(1, sizeof(TokenInfo));
            tokenInfo = tokenInfo->next;
            tokenInfo->type = UNDEFINED;
            tokenInfo->next = NULL;
        }
    }
    fclose(fp);


    printf("DEBUG: readFileSF[scan] - Begin info print at %p\n", infoRoot);
    tokenInfo = infoRoot;
    while(tokenInfo != NULL){
        if(tokenInfo->type == INTEGER){
            printf("  type: %d | line: %d | int: %d | loc: %p\n", tokenInfo->type, tokenInfo->line, tokenInfo->integer, tokenInfo);
        }
        else if(tokenInfo->type == FLOAT){
            printf("  type: %d | line: %d | float: %f | loc: %p\n", tokenInfo->type, tokenInfo->line, tokenInfo->decimal, tokenInfo);
        }else{
            printf("  type: %d | line: %d | text: %s | loc: %p\n", tokenInfo->type, tokenInfo->line, tokenInfo->text, tokenInfo);
        }
        tokenInfo = tokenInfo->next;
        //printf("freeing %p\n", tokenInfo);
        //free(tokenInfo);
    }

    // -----------------
    // readFileSF[group]
    // -----------------
    StructGroup* temp;
    printf("INFO: Beginning readFileSF[group]\n");

    StructGroup* groupRoot;
    groupRoot = sg_alloc();

    StructGroup* structGroup = groupRoot;

    tokenInfo = infoRoot;
    bool skipAdd = false;
    bool alreadyClean = false;
    while(tokenInfo != NULL){
        skipAdd = false;
        alreadyClean = false;
        //printf("token: %d\n", tokenInfo->type);
        if(tokenInfo->type == LEFT_PAREN){
            //printf("child\n");
            structGroup->token.line = tokenInfo->line;
            structGroup->token.type = NO_TYPE;
            structGroup->child = sg_alloc();
            structGroup->child->parent = structGroup;
            structGroup = structGroup->child;
            skipAdd = true;
            alreadyClean = true;
            //printf("parent: %p\n", structGroup->parent);
        }else if(tokenInfo->type == RIGHT_PAREN){
            if(structGroup->parent == NULL){
                printf("ERROR: readFileSF[group] - [line %d] Grouping failed. No opening bracket was put before a closing bracket\n", tokenInfo->line);
                return 1;
            }
            structGroup->previous->next = NULL;
            temp = structGroup;
            structGroup = structGroup->parent;
            free(temp);
            skipAdd = true;
        }
        if(!skipAdd) structGroup->token = *tokenInfo;


        advanceToken();
        if(!alreadyClean){
            structGroup->next = sg_alloc();
            structGroup->next->parent = structGroup->parent;
            structGroup->next->previous = structGroup;
            structGroup = structGroup->next;
        }
        
    }
    structGroup = groupRoot;
    for(int i = 0; structGroup != NULL;){
        printf("\n---GROUP %d---\n", i);
        tokenInfo = &structGroup->token;
        if(tokenInfo->type == INTEGER){
            printf("type: %d | line: %d | int: %d | loc: %p\n", tokenInfo->type, tokenInfo->line, tokenInfo->integer, tokenInfo);
        }
        else if(tokenInfo->type == FLOAT){
            printf("type: %d | line: %d | float: %f | loc: %p\n", tokenInfo->type, tokenInfo->line, tokenInfo->decimal, tokenInfo);
        }else{
            printf("type: %d | line: %d | text: %s | loc: %p\n", tokenInfo->type, tokenInfo->line, tokenInfo->text, tokenInfo);
        }
        if(structGroup->child != NULL){
            printf("\nvv- ENTER CHILD -vv\n");
            structGroup = structGroup->child;
        }else if(structGroup->next != NULL){
            structGroup = structGroup->next;
        }else{
            printf("\n^^- REENTER PARENT -^^\n");
            while (structGroup != NULL && structGroup->next == NULL)
            {
                structGroup = structGroup->parent;
                if(structGroup != NULL && structGroup->next != NULL){
                    structGroup = structGroup->next;
                } 
            }
        }
        i++;
    }
    printf("[DEBUG] readFileSF[group] - Done using [scan] materials. Freeing...\n");
    tokenInfo = infoRoot;
    while(tokenInfo != NULL){
        TokenInfo* safeItem = tokenInfo;
        tokenInfo = tokenInfo->next;
        free(safeItem);
    }
    printf("[DEBUG] readFileSF[group] - Successfully freed all old tokens\n");

    // -----------------
    // readFileSF[associate]
    // -----------------
    printf("INFO: Moving onto readFileSF[associate]...\n");
    
    structGroup = groupRoot;
    bool success = false;
    while (structGroup != NULL)
    {
        success = false;
        //printf("i %d\n", structGroup->token.type);
        if(structGroup->token.type <= COLOUR && structGroup->token.type >= PROPERTY){
            //should be associated
            if(structGroup->next->token.type == NO_TYPE){
                structGroup->next->token = structGroup->token;

                success = true;
            }else{
                printf("[DEBUG] readFileSF[associate] - token: %d\n", structGroup->token.type);
                printf("[DEBUG] readFileSF[associate] - next_token: %d\n", structGroup->next->token.type);
                printf("ERROR: readFileSF[associate] - [line %d] Expected group but got item instead (did you forget parentheses?)\n", structGroup->token.line);
                return 1;
            }
        }else if(structGroup->token.type == MINUS){
            if(structGroup->next->token.type == INTEGER){
                structGroup->next->token.integer = -structGroup->next->token.integer;
            }else if(structGroup->next->token.type == FLOAT){
                structGroup->next->token.decimal = -structGroup->next->token.decimal;
            }else{
                printf("ERROR: readFileSF[associate] - [line %d] Expected number while reading a negative symbol\n", structGroup->token.line);
                return 1;
            }
            success = true;
        }
        if(success){
            if(structGroup->previous != NULL)structGroup->previous->next = structGroup->next;
            if(structGroup == groupRoot) groupRoot = structGroup->next;
            if(structGroup->parent != NULL && structGroup == structGroup->parent->child) structGroup->parent->child = structGroup->next;
            structGroup->next->previous = structGroup->previous;
            temp = structGroup->next;
            free(structGroup);
            structGroup = temp;
        }
        //printf("i %d\n", structGroup->token.type);
        if(structGroup->child != NULL){
            //printf("vv- FOUND CHILD. ENTER CHILD -vv\n");
            structGroup = structGroup->child;
        }else if(structGroup->next != NULL){
            structGroup = structGroup->next;
        }else{
            //printf("^^- REENTER PARENT -^^\n");
            while (structGroup != NULL && structGroup->next == NULL)
            {
                structGroup = structGroup->parent;
                if(structGroup != NULL && structGroup->next != NULL){
                    structGroup = structGroup->next;
                } 
            }
        }
    }

    structGroup = groupRoot;
    for(int i = 0; structGroup != NULL;){
        printf("\n---GROUP %d---\n", i);
        tokenInfo = &structGroup->token;
        if(tokenInfo->type == INTEGER){
            printf("type: %d | line: %d | int: %d | loc: %p\n", tokenInfo->type, tokenInfo->line, tokenInfo->integer, tokenInfo);
        }
        else if(tokenInfo->type == FLOAT){
            printf("type: %d | line: %d | float: %f | loc: %p\n", tokenInfo->type, tokenInfo->line, tokenInfo->decimal, tokenInfo);
        }else{
            printf("type: %d | line: %d | text: DISABLED | loc: %p\n", tokenInfo->type, tokenInfo->line, /*tokenInfo->text,*/ tokenInfo);
        }
        if(structGroup->child != NULL){
            printf("\nvv- ENTER CHILD -vv\n");
            structGroup = structGroup->child;
        }else if(structGroup->next != NULL){
            structGroup = structGroup->next;
        }else{
            printf("\n^^- REENTER PARENT -^^\n");
            while (structGroup != NULL && structGroup->next == NULL)
            {
                structGroup = structGroup->parent;
                if(structGroup != NULL && structGroup->next != NULL){
                    structGroup = structGroup->next;
                } 
            }
        }
        i++;
    }
    
    // -----------------
    // readFileSF[parse]
    // -----------------
    printf("INFO: Finalizing read with readFileSF[parse]...\n");

    BoxCollider2D leverCache[64];
    BoxCollider2D doorCache[64];
    BoxCollider2D portalCache[64];
    //BoxCollider2D wallCache[64];

    int levelColID = 0;
    int ladderID = 0;
    int physObjID = 0;
    int textBoxID = 0;

    int leverID = 0;
    int doorID = 0;
    int portalID = 0;
    *wallImgNum = 0;
    *isMultiplayer = 0; // default
    *wallEnabled = 0x0000;
    *leverFlip = 0x0000000000000000;
    backgroundPath = "@";


    structGroup = groupRoot;
    while(structGroup != NULL){
        //printf("reading...\n");
        switch (structGroup->token.type)
        {
            case WALL_TAG: ; // dumb semicolon required here because the MinGW compiler got mad at me
                //max on wall tags??
                WallTag wallTag = parseWallTag(structGroup);
                wallTags[wallTag.wallId] = wallTag.tag;
                *wallEnabled = *wallEnabled | (wallTag.enabled) << wallTag.wallId;
                break;
            case PHYS_OBJ:
            if(physObjID >= 8){
                    printf("WARNING: readFileSF[parse] - [line %d] Exceeded limit of PhysicsObjects. Skipping...\n", structGroup->token.line);
                }else{
                    physobjects[physObjID] = parsePhysicsObject(structGroup);
                    physObjID++;
                }
                break;
            case TEXT_BOX:
                if(textBoxID >= 2){
                    printf("WARNING: readFileSF[parse] - [line %d] Exceeded limit of TextBoxes. Skipping...\n", structGroup->token.line);
                }else{
                    texts[textBoxID] = parseTextBox(structGroup);
                    textBoxID++;
                }
                break;
            case WALL:
            case LEVEL_COL:
                if(levelColID >= 64){
                    printf("WARNING: readFileSF[parse] - [line %d] Exceeded limit of Level Colliders. Skipping...\n", structGroup->token.line);
                }else{
                    levelCol[levelColID] = parseBoxCollider2D(structGroup);
                    levelColID++;
                }
                break;
            case LADDER_COL:
                if(ladderID >= 16){
                    printf("WARNING: readFileSF[parse] - [line %d] Exceeded limit of Ladders. Skipping...\n", structGroup->token.line);
                }else{
                    ladders[ladderID] = parseBoxCollider2D(structGroup);
                    ladderID++;
                }
                break;
            case LEVER:
                if(levelColID >= 64){
                    printf("WARNING: readFileSF[parse] - [line %d] Exceeded limit of Level Colliders [Levers count towards this]. Skipping...\n", structGroup->token.line);
                }else{
                    leverCache[leverID] = parseBoxCollider2D(structGroup);
                    leverID++;
                    levelColID++;
                }
                break;
            case DOOR:
                if(levelColID >= 64){
                    printf("WARNING: readFileSF[parse] - [line %d] Exceeded limit of Level Colliders [Doors count towards this]. Skipping...\n", structGroup->token.line);
                }else{
                    doorCache[doorID] = parseBoxCollider2D(structGroup);
                    doorID++;
                    levelColID++;
                }
                break;
            case PORTAL:
                if(levelColID >= 64){
                    printf("WARNING: readFileSF[parse] - [line %d] Exceeded limit of Level Colliders [Portals count towards this]. Skipping...\n", structGroup->token.line);
                }else{
                    portalCache[portalID] = parseBoxCollider2D(structGroup);
                    portalID++;
                    levelColID++;
                }
                break;
            case PROPERTY:
                if(structGroup->child == NULL){
                    printf("WARNING: readFileSF[parse] - [line %d] Property had no arguments. Skipping...\n", structGroup->token.line);
                    break;
                }
                if(structGroup->child->token.type != STRING){
                    printf("WARNING: readFileSF[parse] - [line %d] Property had non-string argument as propretyName. Skipping...\n", structGroup->token.line);
                    break;
                }
                StructGroup* argument = structGroup->child->next;
                if(argument == NULL){
                    //error
                    break;
                }
                char* propertyName = structGroup->child->token.text;
                printf("prop name: %s\n", propertyName);
                if(propertyName == NULL){
                    //error
                    break;
                }
                if(TextIsEqual(propertyName, "img")){
                    if(argument->token.type == STRING){
                        if(TextLength(argument->token.text) < 64){
                            TextCopy(levelImagePath, argument->token.text);
                        }else{
                            //error
                        }
                    }else{
                        //error
                    }
                }else if(TextIsEqual(propertyName, "goal")){
                    if(argument->token.type == LEVEL_COL){
                        *goal = parseBoxCollider2D(argument);
                    }else{
                        //error
                    }
                }else if(TextIsEqual(propertyName, "sp")){
                    if(argument->token.type == VECTOR_2){
                        *startingPos = parseVector2(argument);
                        printf("%f; %f\n", startingPos->x, startingPos->y);
                    }else{
                        //error
                        printf("NO\n");
                    }
                }else if(TextIsEqual(propertyName, "sp2")){
                    if(argument->token.type == VECTOR_2){
                        *startingPos2 = parseVector2(argument);
                    }else{
                        //error
                    }
                }else if(TextIsEqual(propertyName, "isMultiplayer")){
                    *isMultiplayer = argument->token.type == TRUE;
                    //error?
                }else if(TextIsEqual(propertyName, "scrollType")){
                    if(argument->token.type == INTEGER){
                        *scrollType = argument->token.integer;
                    }else{
                        //error
                    }
                }else if(TextIsEqual(propertyName, "wallImgNum")){
                    if(argument->token.type == INTEGER){
                        *wallImgNum = argument->token.integer;
                    }else{
                        //error
                    }
                }else if(TextIsEqual(propertyName, "leverFlip")){
                    if(argument->token.type == INTEGER){
                        if(argument->next != NULL)
                            *leverFlip |= argument->next->token.type == TRUE << argument->token.integer;
                        //else error
                    }else if(argument->token.type == NO_TYPE){
                        *leverFlip = parseBoolArray64(argument);
                    }
                    else{
                        //error
                    }
                }else{
                    printf("WARNING: readFileSF[parse] - [line %d] Invalid property name (\"%s\" is not a property). Skipping...\n", structGroup->token.line, propertyName);
                }
                break;
            default:
                printf("WARNING: readFileSF[parse] - [line %d] Received non-struct as parent group. Skipping...\n", structGroup->token.line);
                break;
        }
        structGroup = structGroup->next;
    }
    
    *leverNum = leverID;
    *doorNum = doorID;
    *portalNum = portalID;

    levelColID -= (leverID + doorID + portalID);
    *levelColNum = levelColID;
    *levelTexts = textBoxID;
    *ladderNum = ladderID;
    *physObjNum = physObjID;

    /*for(int i = 0; i < levelColID; i++){
        for(int j = 0; j < *wallImgNum; j++){
            if(levelCol[i].tag == wallTags[j]){
                levelCol[i].enabled = (*wallEnabled << j) & 0x1;
                printf("%d, walltag: %d item: %d tag: %d\n", (*wallEnabled << j) & 0x1, j, i, levelCol[i].tag);
            }
        }
    }*/
    
    for(int i = 0; i < leverID; i++){
        levelCol[levelColID + i] = leverCache[i];
    }
    levelCol += leverID;
    for(int i = 0; i < doorID; i++){
        levelCol[levelColID + i] = doorCache[i];
    }
    levelCol += doorID;
    for(int i = 0; i < portalID; i++){
        levelCol[levelColID + i] = portalCache[i];
    }

    return 0;
}

//TODO - auto wall enable