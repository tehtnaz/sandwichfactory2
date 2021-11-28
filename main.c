#include "animation.h"
#include <stdio.h>
#include "checkColliders.h"
#include "loadNew.h"
#include "velocity.h"
#include "vector2pp.h"
#include "boxPP.h"
#include "rlgl.h"
#include "recpp.h"
#include "gui2.h"

/* Backlog (stuff i dont want to do) (Top to bottom)

* change collisionInfo so that floor is returned as the position and not a collider selection

* VECTOR 2 READING FROM PHYSICS OBJECTS IS BROKEN!!!

* Ladders (just need to draw)
* Allow for crates to give velocity to one another
* Disappearing walls / portals
* Multiple portal instance ppossibility
* Side scrolling - Only if you want to do the original scrolling
* Assign lever to specific door
*/

/* BUGS
* At low frame rates, you cannot grab crates from the side but can from the top
    * THEORY: Is this because the player's hitbox is lower because lower frame rates means higher velocity (no interpol becuase im lazy)
*/

typedef enum GameState{
    STATE_MENU, //everything relating to gameplay is not rendered or processed; menuGui Enabled; level is not loaded
    STATE_ACTIVE,  // all things turned on; gui off
    STATE_PAUSED // everything relating to gameplay except rendering disabled; pauseGui Enabled
}GameState;

typedef enum PlayerMode{
    ONE_PLAYER,
    TWO_PLAYERS
}PlayerMode;

int main(int argc, char* argv[]){

    //init variables and set default values
    int screenWidth = 1920;
    int screenHeight = 1080;
    int screenFPS = 60;
    int gameState = STATE_MENU;
    int playerMode = ONE_PLAYER;

    //generic str
    char str[40];

    //misc.
    char levelImagePath[64];
    levelImagePath[0] = '\0';

    //initialize variables
    Vector2 bgPosition = {0,0};

    //declare prepareLevel
    void prepareLevel(int resolutionMultiplier, 
                    Vector2* playerPos, Vector2* playerPos2, Vector2 startingPos, Vector2 startingPos2, 
                    int selectedLevel, Texture2D* level, char str[40], 
                    int colliderNum, int leverNum, int doorNum, int portalNum, BoxCollider2D Col[25], 
                    TextBox levelText[2], int textNum, 
                    int crateNum, PhysicsObject crate[2], 
                    int ladderNum, BoxCollider2D ladderCol[5], char levelImagePath[64],
                    Animation** doorList, SwitchAnimation** leverList, //bool** door_isClosedList,
                    int customLevel, 
                    //char* levelWallsImgPath, int* pathLength, 
                    int wallNum, Texture2D wallImg[16], int wallTags[16], uint16_t* wallEnabled
                    //Texture* defaultDoor, Texture* leverOn, Texture* leverOff
                    );

    //load colliders and resize starting position and declare which level we start with
    int selectedLevel = 0;
    const int maxLevel = 7; //default level count - 1 (because index 0)

    //player properties
    float velocity = 0.00f;
    float velocity2 = 0.00f;
    bool player_flipX = false;
    bool player2_flipX = false;
    Vector2 playerPos = {0,0};
    Vector2 playerPos2 = {0,0};

    //objects
    BoxCollider2D Col[25];
    TextBox levelText[2];
    PhysicsObject crate[2];
    TriSlope triCol[10];
    Triangle realTri[10];
    BoxCollider2D ladderCol[5];

    //# of objs
    int colliderNum = 0;
    int textNum = 0;
    int ladderNum = 0;
    int crateNum = 0;

    //other level properties
    int leverNum = 0;
    int doorNum = 0;
    int portalNum = 0;
    Vector2 startingPos = {0,0};
    Vector2 startingPos2 = {0,0};

    //debug variables
    bool ColliderDebugMode = false;
    bool CrateDebugMode = false;
    bool showVelocity = false;
    int ShowCollider = 0;

    //Get Window properties
    if(argc == 1){
        //Window properties prompt
        printf("Enter Resolution (recommended: 135, 270, 540, 1080): ");
        scanf("%d", &screenHeight);
        screenWidth = screenHeight / 9.00f * 16.00f;

        printf("Enter desired FPS: ");
        scanf("%d", &screenFPS);

    }else{
        printf("Getting arg1: ");
        screenHeight = parseInt(argv[1], 10);
        screenWidth = screenHeight / 9.00f * 16.00f;

        printf("Getting arg2: ");
        screenFPS = parseInt(argv[2], 10);

        if(screenHeight == 0){
            printf("ERROR: readArgs - screenHeight (argument 1) is not valid\n");
        }
        if(screenFPS == 0){
            printf("ERROR: readArgs - screenFPS (argument 2) is not valid\n");
        }

        if(screenHeight == 0 || screenFPS == 0){
            //Window properties prompt
            printf("Enter Resolution (recommended: 135, 270, 540, 1080): ");
            scanf("%d", &screenHeight);
            screenWidth = screenHeight / 9.00f * 16.00f;

            printf("Enter desired FPS: ");
            scanf("%d", &screenFPS);
        }

        /*if(argv[3] == "1" || argv[3] == "s"){

        }*/
    }


    //image related variables
    const int resolutionMultiplier = screenWidth / 240;
    const int backgroundResMultiplier = screenWidth / 320;
    Texture2D level;
    Texture2D wallImg[16];
    int wallTags[16];
    uint16_t wallEnabled = 0xFFFF;
    int wallNum = 0;

    //game related variables
    const float characterSpeed = 96 / 2.25f * resolutionMultiplier;
    const float gravity = -9.81f * 1.5f * resolutionMultiplier / 8;
    const float jumpHeight = 490.5f * resolutionMultiplier / 8;
    const float frictionCoefficient = 0.98f;
    const float bigFriction = 0.75f;
    const float climbSpeed = -gravity * 13.0f;
    const float ladderGravity = gravity * 0.42f;

    const bool disablePlayerAnim = true;



    //Level prompt
    printf("Type 0 for the regular levels and 1 for a custom level: ");
    int customLevel = 2;
    while(customLevel != 0 && customLevel != 1){
        scanf("%d", &customLevel);
        if(customLevel != 0 && customLevel != 1){
            printf("Enter 0 or 1\n");
        }
    }

    //ask which path then load it
    if(customLevel == 1){
        loadNew(selectedLevel, true, levelImagePath, &startingPos, &startingPos2, Col, levelText, crate, realTri, ladderCol, &colliderNum, &textNum, &ladderNum, &crateNum, &leverNum, &doorNum, &playerMode, &portalNum, wallTags, &wallNum);        
    }

    //start raylib
    InitWindow(screenWidth, screenHeight, "Sandwich Factory");
    rlDisableBackfaceCulling(); // disabled so that triangles aren't culled
    InitAudioDevice();
    SetTargetFPS(screenFPS);
    Image icon = LoadImage("resources/icon_large.png");
    SetWindowIcon(icon);
    //ToggleFullscreen();

    //load animations
    Animation background = assignProperties(0, 0, 10, true, 10, true);
    background.frameBuffer = 0;
    background = getFromFolder(background, "resources/background/", true);

    Animation* doorList = malloc(sizeof(Animation));
    SwitchAnimation* leverList = malloc(sizeof(SwitchAnimation));
    //bool* door_isClosedList = malloc(sizeof(bool));

    //Note: For future, use this method to save memory
    //Texture defaultDoor = getTextureFromFolder("resources/objects/door/", 12);
    //Texture leverOn = LoadTexture("resources/objects/lever/0.png");
    //Texture leverOff = LoadTexture("resources/objects/lever/1.png");

    Animation portal = assignProperties(0, 0, 10, true, 10, true);
    Animation portal2 = assignProperties(0, 0, 10, true, 10, true);

    portal = getFromFolder(portal, "resources/objects/portal/", true);
    portal2 = getFromFolder(portal2, "resources/objects/portal2/", true);

    Animation playerAnim = assignProperties(0, 0, 30, true, 4, false);
    Animation playerAnim_flipped = assignProperties(0, 0, 30, true, 5, false);

    playerAnim = getFromFolder(playerAnim, "resources/objects/playerAnim/", true);
    playerAnim_flipped = getFromFolder(playerAnim, "resources/objects/playerAnim_flip/", true);

    //load images

    Texture2D main_menu = LoadTexture("resources/background/main_menu.png");

    Texture2D player = LoadTexture("resources/objects/player.png");
    Texture2D player_flipped = LoadTexture("resources/objects/player_flip.png");

    Texture2D player2 = LoadTexture("resources/objects/player2.png");
    Texture2D player2_flipped = LoadTexture("resources/objects/player2_flip.png");

    Texture2D crateImage = LoadTexture("resources/objects/crate.png");

    Texture2D ladder = LoadTexture("resources/objects/ladder.png");

    //load sounds

    Sound door_open = LoadSound("resources/sounds/door/open.wav");
    Sound door_close = LoadSound("resources/sounds/door/close.wav");
    Sound portalSound = LoadSound("resources/sounds/portal.wav");

    Music menu_soundtrack = LoadMusicStream("resources/sounds/soundtrack/menu.wav");
    SetMusicVolume(menu_soundtrack, 0.5f);

    Music game_soundtrack = LoadMusicStream("resources/sounds/soundtrack/Funky-Background.wav");
    SetMusicVolume(game_soundtrack, 0.2f);

    //sprite size Vec2 definition
    Vector2 playerSize;
    playerSize.x = player.width * resolutionMultiplier;
    playerSize.y = player.height * resolutionMultiplier;

    Vector2 playerSizeLarge;
    playerSizeLarge.x = playerAnim.spriteSize * resolutionMultiplier;
    playerSizeLarge.y = playerAnim.texture.height * resolutionMultiplier;


    Vector2 inputVelocity = {0,0};
    Vector2 inputVelocity2 = {0,0};

    CollisionInfo collision = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    CollisionInfo collision2 = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


    //prepare level (if custom, unneeded at this point if not custom)
    //cannot be placed before init window since it loads textures and scales


    //Variables for testing purposes
    
    TriColInfo playerTriColInfo = {0,0,0,0};
    TriColInfo playerTriColInfo2 = {0,0,0,0};
    
    triCol[0] = triToTriSlope((Triangle){{100 * 8,100 * 8},{0,119 * 8},{100 * 8,119 * 8}});

    TriColInfo triCollision;
    TriColInfo triCollision2;

    //GUI Variables + assignment

    //fonts
    Font montserrat = LoadFontEx("resources/fonts/Montserrat-Regular.ttf", 100, 0, 0);
    SetTextureFilter(montserrat.texture, TEXTURE_FILTER_BILINEAR);
    Font bade = LoadFontEx("resources/fonts/bade2.ttf", 100, 0, 0);
    SetTextureFilter(bade.texture, TEXTURE_FILTER_BILINEAR);

    //main menu GUI
    GuiText playText = assignGuiText(&montserrat, (Vector2){0,0}, (Vector2){0,0}, "Play", 100, BLACK, 0);
    GuiBox playButton = (GuiBox){RECZERO, &playText, NULL, 1, WHITE, 4, BLACK};
    playButton = offsetGuiBox(playButton, (Vector2){0,125}, (Vector2){250,125}, true, 1920, 1080);

    GuiText titleText = assignGuiText(&bade, (Vector2){0,0}, (Vector2){0,-350}, "SANDWICH", 175, WHITE, 0);
    GuiText titleText2 = assignGuiText(&bade, (Vector2){0,0}, (Vector2){0,-150}, "FACTORY", 175, WHITE, 0);
    
    titleText.center = GetTextCenterGUI(titleText, 1920, 1080);
    titleText2.center = GetTextCenterGUI(titleText2, 1920, 1080);

    playButton = resizeGuiBox(playButton, screenHeight / 1080.0f, true);
    titleText = resizeGuiText(titleText, screenHeight / 1080.0f);
    titleText2 = resizeGuiText(titleText2, screenHeight / 1080.0f);

    //pause game
    GuiBox pauseButton = assignGuiBox(newRec(1810,10,100,100), NULL, NULL, 0, WHITE, 5, BLACK);

    GuiText resumeText = assignGuiText(&montserrat, (Vector2){0,0}, (Vector2){0,0}, "Resume Game", 90, BLACK, 0);
    resumeText.center = GetTextCenterGUI(resumeText, 1920, 1080);

    GuiText menuText = assignGuiText(&montserrat, (Vector2){0,0}, (Vector2){0,0}, "Back to menu", 90, BLACK, 0);
    menuText.center = GetTextCenterGUI(menuText, 1920, 1080);

    GuiBox resumeGame = assignGuiBox(RECZERO, &resumeText, NULL, 1, WHITE, 5, BLACK);
    GuiBox menuButton = assignGuiBox(RECZERO, &menuText, NULL, 1, WHITE, 5, BLACK);
    resumeGame = offsetGuiBox(resumeGame, (Vector2){0,-100}, (Vector2){575,150}, true, 1920, 1080);
    menuButton = offsetGuiBox(menuButton, (Vector2){0,100}, (Vector2){575,150}, true, 1920, 1080);

    resumeText = setGuiTextOrigin(resumeGame, resumeText, true);
    menuText = setGuiTextOrigin(menuButton, menuText, true);

    pauseButton = resizeGuiBox(pauseButton, screenHeight / 1080.0f, false);
    resumeGame = resizeGuiBox(resumeGame, screenHeight / 1080.0f, true);
    menuButton = resizeGuiBox(menuButton, screenHeight / 1080.0f, true);

    if(customLevel == 1){
        gameState = STATE_ACTIVE;
        prepareLevel(resolutionMultiplier, &playerPos, &playerPos2, startingPos, startingPos2, selectedLevel, &level, str, colliderNum, leverNum, doorNum, portalNum, Col, levelText, textNum, crateNum, crate, ladderNum, ladderCol, levelImagePath, &doorList, &leverList, customLevel, wallNum, wallImg, wallTags, &wallEnabled);
    }

    //start sound
    PlayMusicStream(menu_soundtrack);

    while(!WindowShouldClose()){
        if(IsKeyPressed(KEY_F11)){
            ToggleFullscreen();
        }

        //Background music

        UpdateMusicStream(menu_soundtrack);
        UpdateMusicStream(game_soundtrack);
        if(gameState == STATE_MENU){
            if(IsMusicStreamPlaying(game_soundtrack)){
                StopMusicStream(game_soundtrack);
            }
            if(!IsMusicStreamPlaying(menu_soundtrack)){
                PlayMusicStream(menu_soundtrack);
            }
            //printf("mpG: %d; mpM: %d\n", IsMusicPlaying(game_soundtrack), IsMusicPlaying(menu_soundtrack));
        }else{
            if(IsMusicStreamPlaying(menu_soundtrack)){
                StopMusicStream(menu_soundtrack);
            }
            if(!IsMusicStreamPlaying(game_soundtrack)){
                PlayMusicStream(game_soundtrack);
            }
        }
        
        
        //Gameplay related events (collision checking, gravity, animations)
        if(gameState == STATE_ACTIVE){

            //Cycle anims
            for(int i = 0; i < doorNum; i++){
                /*if(doorList[i].isAnimating){
                    if(door_isClosedList[i]){
                        doorList[i] = cycleAnimationBackwards(doorList[i], screenFPS);
                    }else{
                        doorList[i] = cycleAnimation(doorList[i], screenFPS);
                    }
                }*/
                if(doorList[i].isAnimating){
                    if(Col[i + colliderNum + leverNum].enabled){
                        doorList[i] = cycleAnimationBackwards(doorList[i], screenFPS);
                    }else{
                        doorList[i] = cycleAnimation(doorList[i], screenFPS);
                    }
                }
            }
            

            //Prepare collision check and apply gravity

            Rectangle self;
            self.x = playerPos.x;
            self.y = playerPos.y;

            if(!(IsKeyDown(KEY_A) && IsKeyDown(KEY_D)) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) && !disablePlayerAnim){
                self.width = playerSizeLarge.x;
                self.height = playerSizeLarge.y;
            }else{
                self.width = playerSize.x;
                self.height = playerSize.y;
            }
            collision = checkAllColliders(self, true, colliderNum, ladderNum, crateNum, leverNum, doorNum, portalNum, Col, crate, ladderCol);

            triCollision = (TriColInfo){0,0,0,0,-1}; //= isRecInTri(combineVec2(playerPos, playerSize), triCol[0]);
            //playerPos = moveGetPlayerInput(combineVec2(playerPos, playerSize), triCol[0], characterSpeed, &velocity, &triCollision, IsKeyDown(KEY_A), IsKeyDown(KEY_D), &collision);

            if(collision.down == true || triCollision.floor != -1){
                velocity = 0;
                if(triCollision.floor != -1){
                    playerPos.y = triCollision.floor;
                }else if(collision.floor <= colliderNum - 1){
                    playerPos.y = Col[collision.floor].y - playerSize.y + 0.1f;
                }
            }

            if(playerMode == TWO_PLAYERS){
                self = combineVec2(playerPos2, playerSize);

                collision2 = checkAllColliders(self, true, colliderNum, ladderNum, crateNum, leverNum, doorNum, portalNum, Col, crate, ladderCol);

                triCollision2 = (TriColInfo){0,0,0,0,-1}; //= isRecInTri(combineVec2(playerPos2, playerSize), triCol[0]);
                //playerPos2 = moveGetPlayerInput(combineVec2(playerPos2, playerSize), triCol[0], characterSpeed, &velocity2, &triCollision2, IsKeyDown(KEY_LEFT), IsKeyDown(KEY_RIGHT), &collision2);

                if(collision2.down == true || triCollision.floor != -1){
                    velocity2 = 0;
                    if(triCollision2.floor != -1){
                        playerPos2.y = triCollision2.floor;
                    }else if(collision2.floor <= colliderNum - 1){
                        playerPos2.y = Col[collision2.floor].y - playerSize.y + 0.1f;
                    }
                }
            }

            //printf("floor:%f\n", triCollision.floor);

            //playerTriColInfo = isRecInTri((Rectangle){playerPos.x, playerPos.y, playerSize.x, playerSize.y}, triCol[0]);
            //playerPos = modifyPlayerWithTriCol((Rectangle){playerPos.x, playerPos.y, playerSize.x, playerSize.y}, triCol[0], playerTriColInfo, frictionCoefficient, gravity  * (1.0f/(float)screenFPS), &velocity);

            //playerTriColInfo2 = isRecInTri((Rectangle){playerPos2.x, playerPos2.y, playerSize.x, playerSize.y}, triCol[0]);
            //playerPos2 = modifyPlayerWithTriCol((Rectangle){playerPos2.x, playerPos2.y, playerSize.x, playerSize.y}, triCol[0], playerTriColInfo2, frictionCoefficient, gravity  * (1.0f/(float)screenFPS), &velocity2);

            if(IsKeyDown(KEY_F6)){
                collision.inLadder = true;
            }

            //Movement
            //WASD = P1
            // Arrows = P2

            if(IsKeyDown(KEY_W)){
                if(collision.inLadder == false && ((collision.up == false && collision.down == true) || (playerTriColInfo.botLeft || playerTriColInfo.botRight))){
                    // does it make a difference if it's = and not +=? 
                    //it's more messy this way but i feel like it changes the movement and therefore im not messing with it.
                    velocity += jumpHeight; 
                    
                    //jump
                }else if(collision.inLadder && !collision.up){
                    velocity = climbSpeed;
                }
            }
            

            if(IsKeyDown(KEY_A)){
                //left
                player_flipX = true;
                if(collision.left == false){
                    playerPos.x -= characterSpeed / (float)screenFPS;
                }
            }
            if(IsKeyDown(KEY_D)){
                //right
                player_flipX = false;
                if(collision.right == false){
                    playerPos.x += characterSpeed / (float)screenFPS;
                }
            }


            //p2 not updated
            //NOTE(UPDATE): p2 cannot use ladders
            if(playerMode == TWO_PLAYERS){
                if(IsKeyDown(KEY_UP) && ((collision2.up == false && collision2.down == true) || (playerTriColInfo2.botLeft || playerTriColInfo2.botRight))){
                    velocity2 += jumpHeight;
                    //jump
                }
                if(IsKeyDown(KEY_LEFT)){
                    player2_flipX = true;
                    if(collision2.left == false){
                        playerPos2.x -= characterSpeed * (1.0f/(float)screenFPS);
                    }
                }
                if(IsKeyDown(KEY_RIGHT)){
                    player2_flipX = false;
                    if(collision2.right == false){
                        playerPos2.x += characterSpeed * (1.0f/(float)screenFPS);
                    }
                }
            }

            if(IsKeyDown(KEY_LEFT_SHIFT)){
                for(int s = 1; s < 12; s++){
                    if(IsKeyPressed(KEY_F1 + s - 1)){
                        for(int i = 0; i < colliderNum + leverNum + doorNum + portalNum; i++){
                            if(Col[i].tag == s){
                                Col[i].enabled = !Col[i].enabled;
                            }
                        }
                        for(int i = 0; i < ladderNum; i++){
                            if(ladderCol[i].tag == s){
                                ladderCol[i].enabled = !ladderCol[i].enabled;
                            }
                        }
                        printf("Manually toggled tag: %d\n", s);
                    }
                }
            }

            //Interact
            if(IsKeyPressed(KEY_E) && collision.inTrigger > 0){
                if(collision.triggerObjID - colliderNum - leverNum < 0){
                    //interact with lever
                    if(leverList[collision.triggerObjID - colliderNum].currentFrame == 0){
                        leverList[collision.triggerObjID - colliderNum].currentFrame = 1;
                    }else{
                        leverList[collision.triggerObjID - colliderNum].currentFrame = 0;
                    }
                    printf("lever tag = %d\n", collision.inTrigger);
                    for(int i = colliderNum + leverNum; i < colliderNum + leverNum + doorNum; i++){
                        if(Col[i].tag == collision.inTrigger){
                            doorList[i - colliderNum - leverNum].isAnimating = true;
                            if(Col[i].enabled){ // DOBULE CHECK!#!!
                                PlaySound(door_open);
                                //door_isClosedList[i - colliderNum - leverNum] = false;
                                Col[i].enabled = false;
                            }else{
                                PlaySound(door_close);
                                //door_isClosedList[i - colliderNum - leverNum] = true;
                                Col[i].enabled = true;
                            }
                        }
                        printf("t[%d]=%d\n", i, Col[i].tag);

                    }
                    //doorList[collision.triggerObjID - colliderNum].isAnimating = true;
                    //printf("door should be animating %d\n", door_isClosedList[collision.triggerObjID - colliderNum]);
                }else{
                    printf("portal tag = %d\n", collision.inTrigger);
                    //interact with portal
                    bool didActivate = false;
                    for(int i = 0; i < colliderNum + leverNum + doorNum + portalNum; i++){
                        if(Col[i].tag == collision.inTrigger){
                            Col[i].enabled = !Col[i].enabled;
                            didActivate = true;
                        }
                    }
                    for(int i = 0; i < ladderNum; i++){
                        if(ladderCol[i].tag == collision.inTrigger){
                            ladderCol[i].enabled = !ladderCol[i].enabled;
                            didActivate = true;
                        }
                    }
                    for(int i = 0; i < wallNum; i++){
                        if(wallTags[i] == collision.inTrigger){
                            printf("s:");
                            for(int i = 0; i < 16; i++){
                                printf("%d", (wallEnabled >> i) & 1);
                            }
                            printf("\n");
                            wallEnabled = wallEnabled ^ (0b000000000000001 << i);
                            for(int i = 0; i < 16; i++){
                                printf("%d", (wallEnabled >> i) & 1);
                            }
                            printf("\n");
                        }else{
                            printf("%d, %d\n", wallTags[i], collision.inTrigger);
                        }
                    }
                    if(didActivate){
                        PlaySound(portalSound);
                    }
                }
                

            }

            //Player 2 Interact NOTE(UPDATE): not updated for door tags
            if(playerMode == TWO_PLAYERS && IsKeyPressed(KEY_RIGHT_CONTROL) && collision2.inTrigger){
                //interact with lever
                if(leverList[collision2.triggerObjID - colliderNum].currentFrame == 0){
                    leverList[collision2.triggerObjID - colliderNum].currentFrame = 1;
                }else{
                    leverList[collision2.triggerObjID - colliderNum].currentFrame = 0;
                }
                doorList[collision2.triggerObjID - colliderNum].isAnimating = true;
                //printf("door should be animating %d\n", door_isClosedList[collision2.triggerObjID - colliderNum]);
                if(Col[collision2.triggerObjID].enabled){
                    PlaySound(door_open);
                    Col[collision2.triggerObjID].enabled = false;
                    Col[collision2.triggerObjID + leverNum].enabled = false;
                }else{
                    PlaySound(door_close);
                    Col[collision2.triggerObjID].enabled = true;
                    Col[collision2.triggerObjID + leverNum].enabled = true;
                }
            }

            //Reset

            if(playerPos.y > screenHeight || IsKeyPressed(KEY_R)){
                playerPos = startingPos;
            }
            if(playerMode == TWO_PLAYERS && (playerPos2.y > screenHeight || IsKeyPressed(KEY_R))){
                playerPos2 = startingPos2;
            }


            //Check if passed level ending
            if(playerPos.x > screenWidth || (IsKeyPressed(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_TAB))){
                if(customLevel == 0){
                    selectedLevel++;
                    if(selectedLevel > maxLevel){
                        printf("Error: Cannot load level above specified maximum (%d is too large for %d)\n", selectedLevel, maxLevel);
                        playerPos = startingPos;
                        CloseWindow();
                        return 0;
                    }else{
                        printf("Attemping to load level %d because posX: %f > width: %d\n", selectedLevel, playerPos.x, screenWidth);
                        if(loadNew(selectedLevel, false, levelImagePath, &startingPos, &startingPos2, Col, levelText, crate, realTri, ladderCol, &colliderNum, &textNum, &ladderNum, &crateNum, &leverNum, &doorNum, &playerMode, &portalNum, wallTags, &wallNum) == 0){
                            printf("Succesfully loaded\n");
                        }else{
                            printf("Failed loading level.\n");
                            CloseWindow();
                            return 0;
                        }
                        prepareLevel(resolutionMultiplier, &playerPos, &playerPos2, startingPos, startingPos2, selectedLevel, &level, str, colliderNum, leverNum, doorNum, portalNum, Col, levelText, textNum, crateNum, crate, ladderNum, ladderCol, levelImagePath, &doorList, &leverList, customLevel, wallNum, wallImg, wallTags, &wallEnabled);
                    }
                }else{
                    customLevel++;
                    printf("Attemping to load level %d because posX: %f > width: %d\n", selectedLevel, playerPos.x, screenWidth);
                    if(loadNew(selectedLevel, false, levelImagePath, &startingPos, &startingPos2, Col, levelText, crate, realTri, ladderCol, &colliderNum, &textNum, &ladderNum, &crateNum, &leverNum, &doorNum, &playerMode, &portalNum, wallTags, &wallNum) == 0){
                        printf("Succesfully loaded\n");
                    }else{
                        printf("Failed loading level.\n");
                        CloseWindow();
                        return 0;
                    }
                    prepareLevel(resolutionMultiplier, &playerPos, &playerPos2, startingPos, startingPos2, selectedLevel, &level, str, colliderNum, leverNum, doorNum, portalNum, Col, levelText, textNum, crateNum, crate, ladderNum, ladderCol, levelImagePath, &doorList, &leverList, customLevel, wallNum, wallImg, wallTags, &wallEnabled);
                }
            }

            //Velocity management
            if((velocity <= 0 && collision.down == false) || velocity > 0){
                if(collision.inLadder){
                    velocity += ladderGravity * 60.00f / screenFPS;
                }else{
                    velocity += gravity * 60.00f / (float)screenFPS;
                }
                if(!(velocity > 0 && collision.up == true)){
                    playerPos.y -= velocity / (float)screenFPS;
                }else{
                    velocity = 0;
                }
            }

            if(playerMode == TWO_PLAYERS && ((velocity2 <= 0 && collision2.down == false) || velocity2 > 0)){
                velocity2 += gravity * (60.00f/(float)screenFPS);
                if(!(velocity2 > 0 && collision2.up == true)){
                    playerPos2.y -= velocity2 / (float)screenFPS;
                }else{
                    velocity2 = 0;
                }
            }

            //Manage crate collisions
            
            inputVelocity = (Vector2){0,0};
            if(IsKeyDown(KEY_A)){
                inputVelocity.x = -characterSpeed;
            }
            if(IsKeyDown(KEY_D)){
                inputVelocity.x = characterSpeed;
            }
            if(IsKeyDown(KEY_A) && IsKeyDown(KEY_D)){
                inputVelocity.x = 0;
            }
            inputVelocity.y = velocity;

            if(playerMode == TWO_PLAYERS){
                inputVelocity2 = (Vector2){0,0};
                if(IsKeyDown(KEY_LEFT)){
                    inputVelocity2.x = -characterSpeed;
                }
                
                if(IsKeyDown(KEY_RIGHT)){
                    inputVelocity2.x = characterSpeed;
                }
                
                if(IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_RIGHT)){
                    inputVelocity2.x = 0;
                }
                inputVelocity2.y = velocity2;
            }

            if(!(IsKeyDown(KEY_A) && IsKeyDown(KEY_D)) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) && !disablePlayerAnim){
                //p1 large
                crate[0] = updateObject(crate[0], playerPos, playerPos2, playerSizeLarge, playerSize, inputVelocity, inputVelocity2, frictionCoefficient, bigFriction, screenFPS, gravity, 0, player_flipX, resolutionMultiplier, colliderNum, ladderNum, crateNum, leverNum, doorNum, portalNum, Col, crate, ladderCol);
                crate[1] = updateObject(crate[1], playerPos, playerPos2, playerSizeLarge, playerSize, inputVelocity, inputVelocity2, frictionCoefficient, bigFriction, screenFPS, gravity, 1, player_flipX, resolutionMultiplier, colliderNum, ladderNum, crateNum, leverNum, doorNum, portalNum, Col, crate, ladderCol);
            }else{
                crate[0] = updateObject(crate[0], playerPos, playerPos2, playerSize, playerSize, inputVelocity, inputVelocity2, frictionCoefficient, bigFriction, screenFPS, gravity, 0, player_flipX, resolutionMultiplier, colliderNum, ladderNum, crateNum, leverNum, doorNum, portalNum, Col, crate, ladderCol);
                crate[1] = updateObject(crate[1], playerPos, playerPos2, playerSize, playerSize, inputVelocity, inputVelocity2, frictionCoefficient, bigFriction, screenFPS, gravity, 1, player_flipX, resolutionMultiplier, colliderNum, ladderNum, crateNum, leverNum, doorNum, portalNum, Col, crate, ladderCol);
            }

            //Change camera position
            /*Vector2 camInput = addNewVec2(negVec2(playerPos), screenWidth / 2.00f, screenHeight / 2.00f);
            if(!disableCam && camera.smoothingEnabled){
                camera = lerpCamera(camera, camInput.x, camInput.y);
            }*/


            /*
            * Debug Section extra info
            * 0 = None
            * 1 = Left (Red)
            * 2 = Right (Green)
            * 3 = Up (Blue)
            * 4 = Down (Yellow)
            */
            if(IsKeyPressed(KEY_F1)){
                ColliderDebugMode = !ColliderDebugMode;
                //printf("triCol[0].slope = %f; triCol[0].init = %f", triCol[0].slope, triCol[0].init);
            }
            if(IsKeyPressed(KEY_F2)){
                CrateDebugMode = !CrateDebugMode;
            }
            if(IsKeyPressed(KEY_F3)){
                showVelocity = !showVelocity;
            }
            if(IsKeyPressed(KEY_F4)){
                if(ShowCollider > 0){
                    ShowCollider = 0;
                }else{
                    ShowCollider = 1;
                }
            }
            if(IsKeyPressed(KEY_F5)){
                for(int i = 0; i < colliderNum + leverNum + doorNum + portalNum; i++){
                    printf("boxX%d = %d; ", i, Col[i].x / resolutionMultiplier);
                    printf("y = %d; ", Col[i].y / resolutionMultiplier);
                    printf("sX = %d; ", Col[i].sizeX / resolutionMultiplier);
                    printf("sY = %d\n", Col[i].sizeY / resolutionMultiplier);
                    
                }
                printf("lever1ID: %d\n", colliderNum);
                printf("door1ID: %d\n", colliderNum + leverNum);
                printf("portal1ID: %d\n", colliderNum + leverNum + doorNum);
                printf("Col array size: %d\n", colliderNum + leverNum + doorNum + portalNum);
            }
            
            if(IsKeyPressed(KEY_TAB) && ShowCollider > 0){
                if(ShowCollider > colliderNum + leverNum + doorNum + portalNum){
                    ShowCollider = 1;
                }else{
                    ShowCollider++;
                }
            }

        }
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

            //Gameplay related textures
            if(gameState == STATE_ACTIVE || gameState == STATE_PAUSED){
                
                //Background
                if(gameState == STATE_ACTIVE){
                    //active
                    DrawAnimationPro(&background, bgPosition, backgroundResMultiplier, WHITE, screenFPS, CYCLE_FORWARD);
                }else{
                    //paused
                    DrawAnimationPro(&background, bgPosition, backgroundResMultiplier, WHITE, screenFPS, CYCLE_NONE);
                }

                //Level + Text
                DrawTextureEx(level, bgPosition, 0, resolutionMultiplier, WHITE);
                for(int i = 0; i < wallNum; i++){
                    if(((wallEnabled >> i) & 0b1) == 1)
                    DrawTextureEx(wallImg[i], bgPosition, 0, resolutionMultiplier, WHITE);
                    else
                    DrawTextureEx(wallImg[i], bgPosition, 0, resolutionMultiplier, (Color){130, 130, 130, 130});
                }
                for(int i = 0; i < textNum; i++){
                    DrawText(levelText[i].text, levelText[i].x, levelText[i].y, levelText[i].size, levelText[i].colour);
                }

                //Triangles

                //drawTriSlope(triCol[0], RED);

                //Objects in level
                for(int i = 0; i < leverNum; i++){
                    DrawTextureEx(leverList[i].frames[leverList[i].currentFrame], boxToVec2(Col[colliderNum + i]), 0, resolutionMultiplier, WHITE);
                }
                for(int i = 0; i < doorNum; i++){
                    //give pointer directly
                    DrawAnimationPro(doorList+i, boxToVec2(Col[colliderNum + leverNum + i]), resolutionMultiplier, WHITE, screenFPS, CYCLE_NONE);
                }
                if(crateNum > 0){
                    for(int i = 0; i < crateNum; i++){
                        DrawTextureEx(crateImage, crate[i].position, 0, resolutionMultiplier, WHITE);
                    }
                }
                for(int i = colliderNum + leverNum + doorNum; i < colliderNum + leverNum + doorNum + portalNum; i++){
                    DrawAnimationPro(i % 2 == 0 ? &portal : &portal2, boxToVec2(Col[i]), resolutionMultiplier, WHITE, screenFPS, CYCLE_FORWARD);
                }

                //Ladders
                for(int i = 0; i < ladderNum; i++){
                    for(int step = 0; step < (ladderCol[i].sizeY / 3) / resolutionMultiplier; step++){
                        DrawTexturePro(ladder, (Rectangle){0,(step * 3) % 21,7,3}, (Rectangle){ladderCol[i].x, ladderCol[i].y + (step * 3 * resolutionMultiplier), 7 * resolutionMultiplier, 3 * resolutionMultiplier}, (Vector2){0,0}, 0, WHITE);
                    }
                    DrawTexturePro(ladder, (Rectangle){0,0,7,(ladderCol[i].sizeY % (3 * resolutionMultiplier)) / resolutionMultiplier}, (Rectangle){ladderCol[i].x, ladderCol[i].y + ladderCol[i].sizeY - (ladderCol[i].sizeY % (3 * resolutionMultiplier)), 7 * resolutionMultiplier, ladderCol[i].sizeY % (3 * resolutionMultiplier)}, (Vector2){0,0}, 0, WHITE);
                }

                //Player
                if(!(IsKeyDown(KEY_A) && IsKeyDown(KEY_D)) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) && gameState == STATE_ACTIVE && !disablePlayerAnim){
                    if(player_flipX){
                        DrawAnimationPro(&playerAnim_flipped, playerPos, resolutionMultiplier, WHITE, screenFPS, CYCLE_SHAKE);
                    }else{
                        DrawAnimationPro(&playerAnim, playerPos, resolutionMultiplier, WHITE, screenFPS, CYCLE_SHAKE);
                    }
                }else if(gameState == STATE_ACTIVE || inputVelocity.x == 0 || disablePlayerAnim){
                    if(player_flipX){
                        DrawTextureEx(player_flipped, playerPos, 0, resolutionMultiplier, WHITE);
                    }else{
                        DrawTextureEx(player, playerPos, 0, resolutionMultiplier, WHITE);
                    }
                }else{
                    if(inputVelocity.x < 0){
                        DrawAnimationPro(&playerAnim_flipped, playerPos, resolutionMultiplier, WHITE, screenFPS, CYCLE_NONE);
                    }else{
                        DrawAnimationPro(&playerAnim, playerPos, resolutionMultiplier, WHITE, screenFPS, CYCLE_NONE);
                    }
                }

                if(playerMode == TWO_PLAYERS){
                    if(player2_flipX){
                        DrawTextureEx(player2_flipped, playerPos2, 0, resolutionMultiplier, WHITE);
                    }else{
                        DrawTextureEx(player2, playerPos2, 0, resolutionMultiplier, WHITE);
                    }
                }
            }

            //Debug info
            if(gameState == STATE_ACTIVE){
                if(IsKeyDown(KEY_F6)){
                    DrawText("Manual Ladder Enabled", 10, 10, 50, WHITE);
                }
                if(IsKeyPressed(KEY_F7)){
                    for(int i = 0; i < colliderNum + leverNum + doorNum + crateNum + ladderNum + portalNum; i++){
                        printf("%d;", collision.colsTouched[i]);
                    }
                    printf("\n");
                }
                //Debug related - DOES NOT FOLLOW CAM!!
                if(ColliderDebugMode){
                    if(!(IsKeyDown(KEY_A) && IsKeyDown(KEY_D)) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) && !disablePlayerAnim) DrawRectangle(playerPos.x, playerPos.y, playerSizeLarge.x, playerSizeLarge.y, PINK);
                        else DrawRectangle(playerPos.x, playerPos.y, playerSize.x, playerSize.y, PINK);

                    if(triCollision.botRight) DrawCircleV(addVec2(playerPos, playerSize), 3, PURPLE);
                    if(triCollision.topLeft) DrawCircleV(playerPos, 3, PURPLE);
                    if(triCollision.topRight) DrawCircle(playerPos.x + playerSize.x, playerPos.y, 3, PURPLE);
                    if(triCollision.botLeft) DrawCircle(playerPos.x, playerPos.y + playerSize.y, 3, PURPLE);
                    int colsToResize = colliderNum;
                    if(leverNum) colsToResize++;
                    if(doorNum) colsToResize++;
                    for(int i = 0; i < colsToResize; i++){
                        switch(collision.colsTouched[i]){
                            case 0:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, WHITE);
                                break;
                            case 1:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, RED);
                                break;
                            case 2:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, GREEN);
                                break;
                            case 3:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, BLUE);
                                break;
                            case 4:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, YELLOW);
                                break;
                            case 5:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, BROWN);
                                break;
                            case 6:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, GRAY);
                                break;
                        }
                        DrawRectangleLines(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, BLACK);
                    }
                    for(int i = 0; i < ladderNum; i++){
                        if(collision.colsTouched[i+colsToResize] == 6){
                            DrawRectangle(ladderCol[i].x, ladderCol[i].y, ladderCol[i].sizeX, ladderCol[i].sizeY, SKYBLUE);
                        }else{
                            DrawRectangle(ladderCol[i].x, ladderCol[i].y, ladderCol[i].sizeX, ladderCol[i].sizeY, LIGHTGRAY);
                        }
                        DrawRectangleLines(ladderCol[i].x, ladderCol[i].y, ladderCol[i].sizeX, ladderCol[i].sizeY, BLACK);
                    }
                    DrawCircle(playerPos.x + (playerSize.x / 2),triCollision.floor + playerSize.y, 3, PURPLE);
                    DrawCircle(playerPos2.x + (playerSize.x / 2),triCollision2.floor + playerSize.y, 3, PURPLE);
                    drawSlope(triCol[0].slope, triCol[0].init);
                }
                /*if(true){
                    if(objCollision.left){
                        DrawRectangle(playerPos.x, playerPos.y, 4, playerSize.y, PURPLE);
                    }
                    if(objCollision.right){
                        DrawRectangle(playerPos.x + playerSize.x, playerPos.y, 4, playerSize.y, PURPLE);
                    }
                    if(objCollision.up){
                        DrawRectangle(playerPos.x, playerPos.y, playerSize.x, 4, PURPLE);
                    }
                    if(objCollision.down){
                        DrawRectangle(playerPos.x, playerPos.y + playerSize.y, playerSize.x + 4, 4, PURPLE);
                    }
                }*/
                if(showVelocity){
                    sprintf(str, "%f", velocity);
                    DrawText(str, 0, 50, 100, WHITE);
                }
                if(ShowCollider > 0){
                    DrawRectangle(Col[ShowCollider - 1].x, Col[ShowCollider - 1].y, Col[ShowCollider - 1].sizeX, Col[ShowCollider - 1].sizeY, GREEN);
                    sprintf(str, "%f", playerPos.y + playerSize.y);
                    DrawText(str, 0, 0, 100, GREEN);
                    sprintf(str, "%d", Col[ShowCollider - 1].y);
                    DrawText(str, 0, 100, 100, GREEN);
                    sprintf(str, "%d", Col[ShowCollider - 1].y + Col[ShowCollider - 1].sizeY);
                    DrawText(str, 0, 200, 100, GREEN);
                }
                if(CrateDebugMode){
                    sprintf(str, "%f", crate[0].velocity.x);
                    DrawText(str, 0, 0, 100, RED);
                    sprintf(str, "%f", inputVelocity.x);
                    DrawText(str, 0, 100, 100, RED);
                    sprintf(str, "%f", inputVelocity2.x);
                    DrawText(str, 0, 200, 100, RED);
                }
            }

            //In-Game GUI
            if(gameState == STATE_ACTIVE){

                //pause button
                renderGuiBox(pauseButton, false);

                if(IsMouseButtonDown(0) && isMouseInGuiBox(pauseButton)){
                    gameState = STATE_PAUSED;
                }

            }

            //Menu GUI
            if(gameState == STATE_MENU){

                //background
                DrawTextureEx(main_menu, bgPosition, 0, resolutionMultiplier, WHITE);

                //buttons
                playText = setGuiTextOrigin(playButton, playText, true);
                renderGuiBox(playButton, true);


                //title
                renderGuiText(titleText);
                renderGuiText(titleText2);


                if(IsMouseButtonPressed(0) && isMouseInGuiBox(playButton)){
                    loadNew(selectedLevel, false, levelImagePath, &startingPos, &startingPos2, Col, levelText, crate, realTri, ladderCol, &colliderNum, &textNum, &ladderNum, &crateNum, &leverNum, &doorNum, &playerMode, &portalNum, wallTags, &wallNum);
                    prepareLevel(resolutionMultiplier, &playerPos, &playerPos2, startingPos, startingPos2, selectedLevel, &level, str, colliderNum, leverNum, doorNum, portalNum, Col, levelText, textNum, crateNum, crate, ladderNum, ladderCol, levelImagePath, &doorList, &leverList, customLevel, wallNum, wallImg, wallTags, &wallEnabled);
                    gameState = STATE_ACTIVE;
                }
            }
            
            //Pause GUI

            if(gameState == STATE_PAUSED){

                renderGuiBox(resumeGame, true);
                renderGuiBox(menuButton, true);

                if(IsMouseButtonPressed(0) && isMouseInGuiBox(resumeGame)){
                    gameState = STATE_ACTIVE;
                }

                if(IsMouseButtonPressed(0) && isMouseInGuiBox(menuButton)){
                    gameState = STATE_MENU;
                }
            }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void prepareLevel(int resolutionMultiplier, 
                    Vector2* playerPos, Vector2* playerPos2, Vector2 startingPos, Vector2 startingPos2, 
                    int selectedLevel, Texture2D* level, char str[40], 
                    int colliderNum, int leverNum, int doorNum, int portalNum, BoxCollider2D Col[25], 
                    TextBox levelText[2], int textNum, 
                    int crateNum, PhysicsObject crate[2], 
                    int ladderNum, BoxCollider2D ladderCol[5], char levelImagePath[64],
                    Animation** doorList, SwitchAnimation** leverList, //bool** door_isClosedList,
                    int customLevel, 
                    //char* levelWallsImgPath, int* pathLength, 
                    int wallNum, Texture2D wallImg[16], int wallTags[16], uint16_t* wallEnabled
                    //Texture* defaultDoor, Texture* leverOn, Texture* leverOff
                    ){
    printf("prepareLevel: Preparing level...\n");
    *wallEnabled = 0xFFFF;
    //load level image
    //char* tempString;
    //int processedChars = 0;
    if(customLevel == 0){
        sprintf(str, "resources/levels/%d.png", selectedLevel + 1);
        printf("prepareLevel: Attemping to load image: %s\n", str);
        *level = LoadTexture(str);
        for(int i = 0; i < wallNum; i++){
            wallImg[i] = LoadTexture(TextFormat("resources/levels/%d_walls/%d.png", selectedLevel + 1, i));
            //(*wallImg)[i] = LoadTexture(TextFormat("resources/%d_walls/%d.png", selectedLevel + 1, i));
        }
    }else{
        printf("prepareLevel: customLevel enabled. Attemping to load image from levelImagePath. If the program crashes and no other messages pop up, make sure you've specified the img property\n");
        if(levelImagePath[TextLength(levelImagePath) - 1] == '/'){
            sprintf(str, "custom_levels/%s%d.png", levelImagePath, customLevel - 1);
        }else{
            sprintf(str, "custom_levels/%s", levelImagePath);
            printf("prepareLevel: Attemping to load custom image: %s\n", str);
            *level = LoadTexture(str);
        }
        for(int i = 0; i < wallNum; i++){
            wallImg[i] = LoadTexture(TextFormat("custom_levels/%s_walls/%d.png", levelImagePath, i));
            //(*wallImg)[i] = LoadTexture(TextFormat("custom_levels/%s_walls/%d.png", levelImagePath, i));
        }
        /*for(int i = 0; i < wallNum; i++){
            tempString = malloc(sizeof(int) * (pathLength[i] + 1));
            for(int s = 0; s < pathLength[i]; s++){
                tempString[s] = levelWallsImgPath[s + processedChars];
            }
            processedChars += pathLength[i];
            tempString[pathLength[i] + 1] = '\0';
            (*wallImg)[i] = LoadTexture(TextFormat("custom_levels/%s", tempString));
        }*/
    }
    


    //Resize doors + levers lists
    //Uses double-pointers so that we reference the locations of the actual arrays and not the ones given to the function
    printf("prepareLevel: Resizing lists...\n");
    printf("prepareLevel: doorList size: %I64d\n", sizeof(Animation) * doorNum);
    *doorList = (Animation*)realloc(*doorList, sizeof(Animation) * doorNum);

    //printf("prepareLevel: door_isClosedList size: %I64d\n", sizeof(bool) * doorNum);
    //*door_isClosedList = (bool*)realloc(*door_isClosedList, sizeof(bool) * doorNum);

    printf("prepareLevel: Setting default values for new lists...\n");
    for(int i = 0; i < doorNum; i++){
        printf("prepareLevel: prepareLevel: test1: %p\n", *doorList);
        (*doorList)[i] = (Animation){0,0,0,0,0,0,0,0,0,0,0,0,0};
        printf("prepareLevel: hasn't crashed\n");
        (*doorList)[i] = assignProperties(0, 0, 60, false, 12, false);

        (*doorList)[i] = getFromFolder((*doorList)[i], "resources/objects/door/", true);
        //(*doorList)[i].texture = *defaultDoor;

        //(*door_isClosedList)[i] = true;
        printf("prepareLevel: default vals - Completed obj %d\n", i);
    }

    printf("prepareLevel: leverList size: %I64d\n", sizeof(SwitchAnimation) * leverNum);
    *leverList = (SwitchAnimation*)realloc(*leverList, sizeof(SwitchAnimation) * leverNum);
    for(int i = 0; i < leverNum; i++){
        (*leverList)[i] = switchAssignProperties(0, 10, false);

        (*leverList)[i] = switchGetFromFolder((*leverList)[i], "resources/objects/lever/");
        //(*leverList)[i].frames[0] = *leverOff;
        //(*leverList)[i].frames[1] = *leverOn;
    }

    //resize Colliders according to resolution
    int colsToResize = colliderNum + leverNum + doorNum + portalNum;
    for(int i = 0; i < colsToResize; i++){
        Col[i].x *= resolutionMultiplier;
        Col[i].y *= resolutionMultiplier;
        Col[i].sizeX *= resolutionMultiplier;
        Col[i].sizeY *= resolutionMultiplier;
        printf("%d\n",Col[i].x + Col[i].sizeX);
    }

    //resize ladders

    printf("prepareLevel: ladderNum: %d\n", ladderNum);

    for(int i = 0; i < ladderNum; i++){
        printf("%d;%d;%d;%d\n", ladderCol[i].x, ladderCol[i].y, ladderCol[i].sizeX, ladderCol[i].sizeY);
        ladderCol[i].x *= resolutionMultiplier;
        ladderCol[i].y *= resolutionMultiplier;
        ladderCol[i].sizeX *= resolutionMultiplier;
        ladderCol[i].sizeY *= resolutionMultiplier;
        printf("%d;%d;%d;%d\n", ladderCol[i].x, ladderCol[i].y, ladderCol[i].sizeX, ladderCol[i].sizeY);
    }

    //resize levelText according to resolution
    
    for(int i = 0; i < textNum; i++){
        levelText[i].x *= resolutionMultiplier;
        levelText[i].y *= resolutionMultiplier;
        levelText[i].size *= resolutionMultiplier;
    }
    Vector2 tempPos = startingPos;
    tempPos.x *= resolutionMultiplier;
    tempPos.y *= resolutionMultiplier;
    *playerPos = tempPos;

    tempPos = startingPos2;
    tempPos.x *= resolutionMultiplier;
    tempPos.y *= resolutionMultiplier;
    *playerPos2 = tempPos;

    for(int i = 0; i < crateNum; i++){
        crate[i].position.x *= resolutionMultiplier;
        crate[i].position.y *= resolutionMultiplier;
        crate[i].sizeX *= resolutionMultiplier;
        crate[i].sizeY *= resolutionMultiplier;
        crate[i].velocity.x = 0;
        crate[i].velocity.y = 0;
    }

    /*camera.position.x = 0;
    camera.position.y = 0;

    if(disableCam){
        camera.maxCamera.x = 0;
        camera.maxCamera.y = 0;
        camera.minCamera.x = 0;
        camera.minCamera.y = 0;
        camera.smoothingEnabled = false;
    }*/
}
    