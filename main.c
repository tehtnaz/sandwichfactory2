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


#define GHOST (Color){130, 130, 130, 130}


/* Backlog (stuff i dont want to do) (Top to bottom)
* change collisionInfo so that floor is returned as the position and not a collider selection (allows integration with triangle collisions)
* Allow for crates to give velocity to one another
*/

/* BUGS
* At low frame rates, you cannot grab crates from the side but can from the top
    * THEORY: Is this because the player's hitbox is lower because lower frame rates means higher velocity (no interpol becuase im lazy)
*/

typedef enum GameState{
    STATE_MENU, //everything relating to gameplay is not rendered or processed; menuGui Enabled; level is not loaded
    STATE_ACTIVE,  // all things turned on; gui off
    STATE_PAUSED, // everything relating to gameplay except rendering disabled; pauseGui Enabled
    STATE_END, // render endscreen and thats it
    STATE_CUTSCENE
}GameState;

typedef enum PlayerMode{
    ONE_PLAYER,
    TWO_PLAYERS
}PlayerMode;

// Colliders / Objects
static BoxCollider2D Col[64];
static int colliderNum = 0;

static TextBox levelText[2];
static int textNum = 0;

static PhysicsObject crate[8];
static int crateNum = 0;

//static TriSlope triCol[10];
//static Triangle realTri[10];

static BoxCollider2D ladderCol[16];
static int ladderNum = 0;

//other level properties
static int leverNum = 0;
static int doorNum = 0;
static int portalNum = 0;
static Vector2 startingPos = {0,0};
static Vector2 startingPos2 = {0,0};
static char levelPath[128];

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
    char backgroundPath[64];
    levelImagePath[0] = '\0';

    //initialize variables
    Vector2 bgPosition = {0,0};

    //declare prepareLevel
    void prepareLevel(int customLevel, int selectedLevel, int resolutionMultiplier,
                    Vector2* playerPos, Vector2* playerPos2,
                    Texture2D* level, char levelImagePath[64],
                    Animation** doorList, SwitchAnimation** leverList,
                    int wallNum, Texture2D wallImg[16], BoxCollider2D* goal, uint64_t leverFlip, char levelPath[128]);

    //load colliders and resize starting position and declare which level we start with
    int selectedLevel = 0;
    const int maxLevel = 11; //default level count - 1 (because index 0)

    //player properties
    float velocity = 0.00f;
    float velocity2 = 0.00f;
    bool player_flipX = false;
    bool player2_flipX = false;
    Vector2 playerPos = {0,0};
    Vector2 playerPos2 = {0,0};

    //debug variables
    bool ColliderDebugMode = false;
    bool CrateDebugMode = false;
    bool showVelocity = false;
    int ShowCollider = 0;

    //Get Window properties
    if(argc != 3){
        //Window properties prompt
        printf("Enter Resolution (recommended: 135, 270, 540, 1080): ");
        fgets(str, 40, stdin);
        screenHeight = TextToInteger(str);
        screenWidth = screenHeight / 9.00f * 16.00f;

        printf("Enter desired FPS: ");
        fgets(str, 40, stdin);
        screenFPS = TextToInteger(str);

    }else{
        printf("Getting arg1: ");
        screenHeight = parseInt(argv[1]);
        screenWidth = screenHeight / 9.00f * 16.00f;

        printf("Getting arg2: ");
        screenFPS = parseInt(argv[2]);

        if(screenHeight == 0){
            printf("ERROR: readArgs - screenHeight (argument 1) is not valid\n");
        }
        if(screenFPS == 0){
            printf("ERROR: readArgs - screenFPS (argument 2) is not valid\n");
        }

        if(screenHeight == 0 || screenFPS == 0){
            //Window properties prompt
            printf("Enter Resolution (recommended: 135, 270, 540, 1080): ");
            fgets(str, 40, stdin);
            screenHeight = TextToInteger(str);
            screenWidth = screenHeight / 9.00f * 16.00f;

            printf("Enter desired FPS: ");
            fgets(str, 40, stdin);
            screenFPS = TextToInteger(str);
        }
    }


    //image related variables
    const int resolutionMultiplier = screenWidth / 240;
    const int backgroundResMultiplier = screenWidth / 320;
    Texture2D level;
    Texture2D wallImg[16];
    int wallTags[16];
    uint16_t wallEnabled = 0xFFFF;
    int wallNum = 0;
    uint64_t leverFlip = 0x0000000000000000;
    // 0 = None
    // 1 = Scroll left-right
    // 2 = Scroll top-down
    // 3 = Scroll all round
    int scrollType = 0;

    //game related variables
    const float characterSpeed = 96 / 2.25f * resolutionMultiplier;
    const float gravity = -9.81f * 1.5f * resolutionMultiplier / 8;
    const float jumpHeight = 490.5f * resolutionMultiplier / 8;
    const float frictionCoefficient = 0.98f;
    const float bigFriction = 0.75f;
    const float climbSpeed = -gravity * 13.0f;
    const float ladderGravity = gravity * 0.42f;
    BoxCollider2D goal = {0};

    const bool disablePlayerAnim = true;

    Camera2D camera = {0};
    camera.rotation = 0;
    camera.zoom = 1.0f;

    int customLevel = 2;
    if(argc > 3){
        printf("Loading level from arg 3\n");
        customLevel = readFileSF(argv[3], levelImagePath, backgroundPath, &startingPos, &startingPos2, Col, ladderCol, levelText, crate, &textNum, &colliderNum, &ladderNum, &crateNum, &leverNum, &doorNum, &playerMode, &portalNum, wallTags, &wallNum, &wallEnabled, &goal, &scrollType, &leverFlip);
        if(customLevel == 0){
            customLevel = 3;
        }
    }

    if(customLevel == 2){
        //Level prompt
        printf("Type 0 for the regular levels and 1 for a custom level: ");
        while(customLevel != 0 && customLevel != 1){
            fgets(str, 40, stdin);
            sscanf(str, "%d", &customLevel);
            if(customLevel != 0 && customLevel != 1){
                printf("Enter 0 or 1: \n");
            }
        }
    }

    //ask which path then load it
    if(customLevel == 1){
        loadNew(customLevel, true, levelImagePath, backgroundPath, &startingPos, &startingPos2, Col, levelText, crate, ladderCol, &colliderNum, &textNum, &ladderNum, &crateNum, &leverNum, &doorNum, &playerMode, &portalNum, wallTags, &wallNum, &wallEnabled, &goal, &scrollType, &leverFlip, levelPath);        
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

    Texture2D end_screen = LoadTexture("resources/endscreen.png");

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

    CollisionInfo collision = {0};
    CollisionInfo collision2 = {0};


    //prepare level (if custom, unneeded at this point if not custom)
    //cannot be placed before init window since it loads textures and scales


    //Variables for testing purposes
    
    //TriColInfo playerTriColInfo = {0,0,0,0};
    //TriColInfo playerTriColInfo2 = {0,0,0,0};
    
    //triCol[0] = triToTriSlope((Triangle){{100 * 8,100 * 8},{0,119 * 8},{100 * 8,119 * 8}});

    //TriColInfo triCollision;
    //TriColInfo triCollision2;

    //GUI Variables + assignment

        //fonts
        Font montserrat = LoadFontEx("resources/fonts/Montserrat-Regular.ttf", 100, 0, 0);
        SetTextureFilter(montserrat.texture, TEXTURE_FILTER_BILINEAR);
        Font bade = LoadFontEx("resources/fonts/bade2.ttf", 100, 0, 0);
        SetTextureFilter(bade.texture, TEXTURE_FILTER_BILINEAR);

        //main menu GUI
        // 0 = none
        // 1 = page 1
        // 2 = page 2
        int levelSelectVisibilityMode = 0;
        GuiText playText = assignGuiText(&montserrat, (Vector2){0,0}, (Vector2){0,0}, "Play", 100, BLACK, 0);
        GuiBox playButton = (GuiBox){RECZERO, &playText, NULL, 1, WHITE, 4, BLACK};
        playButton = offsetGuiBox(playButton, (Vector2){-150,125}, (Vector2){250,125}, true, 1920, 1080);
        playButton = resizeGuiBox(playButton, screenHeight / 1080.0f, true);

        GuiText selectText = assignGuiText(&montserrat, (Vector2){0,0}, (Vector2){0,-30}, "Level", 75, BLACK, 0);
        GuiText selectText2 = assignGuiText(&montserrat, (Vector2){0,0}, (Vector2){0,30}, "Select", 75, BLACK, 0);
        GuiBox selectButton = (GuiBox){RECZERO, &selectText, &selectText2, 2, WHITE, 4, BLACK};
        selectButton = offsetGuiBox(selectButton, (Vector2){150,125}, (Vector2){250,125}, true, 1920, 1080);
        selectButton = resizeGuiBox(selectButton, screenHeight / 1080.0f, true);

        GuiBox levelSelect[12];
        GuiText levelSelectText[12];
        for(int i = 0; i < 12; i++){
            levelSelectText[i] = assignGuiText(&montserrat, (Vector2){0,0}, (Vector2){0,0}, TextFormat("Level %d", i + 1), 75, BLACK, 0);
            levelSelect[i] = (GuiBox){RECZERO, &(levelSelectText[i]), NULL, 1, WHITE, 4, BLACK};
            levelSelect[i] = offsetGuiBox(levelSelect[i], (Vector2){-150 + (300 * (i % 3)),100 + (175 * ((i / 3) % 2))}, (Vector2){250,125}, true, 1920, 1080);
            levelSelect[i] = resizeGuiBox(levelSelect[i], screenHeight / 1080.0f, true);
        }

        GuiText backText = assignGuiText(&montserrat, (Vector2){0,0}, (Vector2){0,0}, "Back", 100, BLACK, 0);
        GuiBox backButton = (GuiBox){RECZERO, &backText, NULL, 1, WHITE, 4, BLACK};
        backButton = offsetGuiBox(backButton, (Vector2){-475,100}, (Vector2){250,125}, true, 1920, 1080);
        backButton = resizeGuiBox(backButton, screenHeight / 1080.0f, true);

        GuiText nextText = assignGuiText(&montserrat, (Vector2){0,0}, (Vector2){0,0}, "Next", 100, BLACK, 0);
        GuiBox nextButton = (GuiBox){RECZERO, &nextText, NULL, 1, WHITE, 4, BLACK};
        nextButton = offsetGuiBox(nextButton, (Vector2){-475,275}, (Vector2){250,125}, true, 1920, 1080);
        nextButton = resizeGuiBox(nextButton, screenHeight / 1080.0f, true);


        GuiText titleText = assignGuiText(&bade, (Vector2){0,0}, (Vector2){0,-350}, "SANDWICH", 175, WHITE, 0);
        GuiText titleText2 = assignGuiText(&bade, (Vector2){0,0}, (Vector2){0,-150}, "FACTORY", 175, WHITE, 0);
        
        titleText.center = GetTextCenterGUI(titleText, 1920, 1080);
        titleText2.center = GetTextCenterGUI(titleText2, 1920, 1080);
        
        titleText = resizeGuiText(titleText, screenHeight / 1080.0f);
        titleText2 = resizeGuiText(titleText2, screenHeight / 1080.0f);



        //pause game
        GuiImg pauseIcon = assignGuiImg(LoadTexture("resources/boring_PauseButton.png"), (Vector2){0,0}, (Vector2){1812,12}, 6);
        //GuiImg playIcon = assignGuiImg(LoadTexture("resources/playButton.png"), (Vector2){0,0}, )
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
        pauseIcon = resizeGuiImg(pauseIcon, screenHeight / 1080.0f);


    //prepare level (if custom, unneeded at this point if not custom)
    //cannot be placed before init window since it loads textures and scales
    if(customLevel == 1 || customLevel == 3){
        gameState = STATE_ACTIVE;
        prepareLevel(customLevel, selectedLevel, resolutionMultiplier, &playerPos, &playerPos2, &level, levelImagePath, &doorList, &leverList, wallNum, wallImg, &goal, leverFlip, levelPath);
    }

    //start sound
    PlayMusicStream(menu_soundtrack);

    //cutscene graphics

    float timer = 0;
    bool anyButtonPrompt = false;
    Animation cutscene = assignProperties(0, 0, 0, true, 13, false);
    cutscene = getFromFolder(cutscene, "resources/cutscene/", true);

    Font defaultFont = GetFontDefault();
    GuiText promptText = assignGuiText(&defaultFont, (Vector2){0,0}, (Vector2){0,500}, "Press Space to skip", 20, WHITE, 0);
    promptText.center = GetTextCenterGUI(promptText, 1920, 1080);
    promptText = resizeGuiText(promptText, screenHeight / 1080.0f);

    while(!WindowShouldClose()){
        if(IsKeyPressed(KEY_F11)){
            ToggleFullscreen();
        }

        //Background music

        UpdateMusicStream(menu_soundtrack);
        UpdateMusicStream(game_soundtrack);
        if(gameState == STATE_MENU || gameState == STATE_END || gameState == STATE_CUTSCENE){
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

            //Cycle door anims
            for(int i = 0; i < doorNum; i++){
                if(doorList[i].isAnimating){
                    if(Col[i + colliderNum + leverNum].enabled){
                        doorList[i] = cycleAnimationBackwards(doorList[i]);
                    }else{
                        doorList[i] = cycleAnimation(doorList[i]);
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

            //triCollision = (TriColInfo){0,0,0,0,-1}; //= isRecInTri(combineVec2(playerPos, playerSize), triCol[0]);
            //playerPos = moveGetPlayerInput(combineVec2(playerPos, playerSize), triCol[0], characterSpeed, &velocity, &triCollision, IsKeyDown(KEY_A), IsKeyDown(KEY_D), &collision);

            //if(collision.down == true || triCollision.floor != -1){
            if(collision.down == true){
                velocity = 0;
                /*if(triCollision.floor != -1){
                    playerPos.y = triCollision.floor;
                }else */
                if(collision.floor <= colliderNum - 1){
                    playerPos.y = Col[collision.floor].y - playerSize.y + 0.1f;
                }
            }

            if(playerMode == TWO_PLAYERS){
                self = combineVec2(playerPos2, playerSize);

                collision2 = checkAllColliders(self, true, colliderNum, ladderNum, crateNum, leverNum, doorNum, portalNum, Col, crate, ladderCol);

                //triCollision2 = (TriColInfo){0,0,0,0,-1}; //= isRecInTri(combineVec2(playerPos2, playerSize), triCol[0]);
                //playerPos2 = moveGetPlayerInput(combineVec2(playerPos2, playerSize), triCol[0], characterSpeed, &velocity2, &triCollision2, IsKeyDown(KEY_LEFT), IsKeyDown(KEY_RIGHT), &collision2);

                //if(collision2.down == true || triCollision2.floor != -1){
                if(collision2.down == true){
                    velocity2 = 0;
                    /*if(triCollision2.floor != -1){
                        playerPos2.y = triCollision2.floor;
                    }else */
                    if(collision2.floor <= colliderNum - 1){
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
                collision2.inLadder = true;
            }

            //Movement
            //WASD = P1
            // Arrows = P2

            if(IsKeyDown(KEY_W)){
                //if(collision.inLadder == false && ((collision.up == false && collision.down == true) || (playerTriColInfo.botLeft || playerTriColInfo.botRight))){
                if(collision.inLadder == false && collision.up == false && collision.down == true){
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

            if(playerMode == TWO_PLAYERS){
                if(IsKeyDown(KEY_UP)){
                    //if(collision2.inLadder == false && ((collision2.up == false && collision2.down == true) || (playerTriColInfo2.botLeft || playerTriColInfo2.botRight))){
                    if(collision2.inLadder == false && collision2.up == false && collision2.down == true){
                        // does it make a difference if it's = and not +=? 
                        //it's more messy this way but i feel like it changes the movement and therefore im not messing with it.
                        velocity2 += jumpHeight; 
                        
                        //jump
                    }else if(collision2.inLadder && !collision2.up){
                        velocity2 = climbSpeed;
                    }
                }
                if(IsKeyDown(KEY_LEFT)){
                    player2_flipX = true;
                    if(collision2.left == false){
                        playerPos2.x -= characterSpeed / (float)screenFPS;
                    }
                }
                if(IsKeyDown(KEY_RIGHT)){
                    player2_flipX = false;
                    if(collision2.right == false){
                        playerPos2.x += characterSpeed / (float)screenFPS;
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
                            if(Col[i].enabled){
                                PlaySound(door_open);
                                Col[i].enabled = false;
                            }else{
                                PlaySound(door_close);
                                Col[i].enabled = true;
                            }
                        }
                        printf("p1:t[%d]=%d\n", i, Col[i].tag);

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
                            printf("p1s:");
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

            if(playerMode == TWO_PLAYERS && IsKeyPressed(KEY_RIGHT_CONTROL) && collision2.inTrigger > 0){
                if(collision2.triggerObjID - colliderNum - leverNum < 0){
                    //interact with lever
                    if(leverList[collision2.triggerObjID - colliderNum].currentFrame == 0){
                        leverList[collision2.triggerObjID - colliderNum].currentFrame = 1;
                    }else{
                        leverList[collision2.triggerObjID - colliderNum].currentFrame = 0;
                    }
                    printf("lever tag = %d\n", collision2.inTrigger);
                    for(int i = colliderNum + leverNum; i < colliderNum + leverNum + doorNum; i++){
                        if(Col[i].tag == collision2.inTrigger){
                            doorList[i - colliderNum - leverNum].isAnimating = true;
                            if(Col[i].enabled){
                                PlaySound(door_open);
                                Col[i].enabled = false;
                            }else{
                                PlaySound(door_close);
                                Col[i].enabled = true;
                            }
                        }
                        printf("p2:t[%d]=%d\n", i, Col[i].tag);

                    }
                    //doorList[collision2.triggerObjID - colliderNum].isAnimating = true;
                    //printf("door should be animating %d\n", door_isClosedList[collision2.triggerObjID - colliderNum]);
                }else{
                    printf("portal tag = %d\n", collision2.inTrigger);
                    //interact with portal
                    bool didActivate = false;
                    for(int i = 0; i < colliderNum + leverNum + doorNum + portalNum; i++){
                        if(Col[i].tag == collision2.inTrigger){
                            Col[i].enabled = !Col[i].enabled;
                            didActivate = true;
                        }
                    }
                    for(int i = 0; i < ladderNum; i++){
                        if(ladderCol[i].tag == collision2.inTrigger){
                            ladderCol[i].enabled = !ladderCol[i].enabled;
                            didActivate = true;
                        }
                    }
                    for(int i = 0; i < wallNum; i++){
                        if(wallTags[i] == collision2.inTrigger){
                            printf("p2s:");
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
                            printf("%d, %d\n", wallTags[i], collision2.inTrigger);
                        }
                    }
                    if(didActivate){
                        PlaySound(portalSound);
                    }
                }
                
            }

            //Reset

            if(playerPos.y > level.height * resolutionMultiplier || IsKeyPressed(KEY_R)){
                playerPos = startingPos;
            }
            if(playerMode == TWO_PLAYERS && (playerPos2.y > level.height * resolutionMultiplier || IsKeyPressed(KEY_R))){
                playerPos2 = startingPos2;
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
                if(collision2.inLadder){
                    velocity2 += ladderGravity * 60.00f / screenFPS;
                }else{
                    velocity2 += gravity * 60.00f / (float)screenFPS;
                }
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


            //Check if passed level ending (player 2 is not checked)
            if(f_checkCollider(combineVec2(playerPos, playerSize), boxToRec(goal), true, true) || (IsKeyPressed(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_TAB))){
                for(int i = 0; i < doorNum; i++){
                    printf("Attempting to unload door texture; %d\n", i);
                    UnloadTexture(doorList[i].texture);
                }
                for(int i = 0; i < leverNum; i++){
                    printf("Attempting to unload lever textures; %d\n", i);
                    UnloadTexture(leverList[i].frames[0]);
                    UnloadTexture(leverList[i].frames[1]);
                }
                UnloadTexture(level);
                for(int i = 0; i < wallNum; i++){
                    UnloadTexture(wallImg[i]);
                }
                if(customLevel == 0){
                    selectedLevel++;
                    if(selectedLevel > maxLevel){
                        printf("Note: Reached end of normals levels (%d is too large for %d)\n", selectedLevel, maxLevel);
                        playerPos = startingPos;
                        gameState = STATE_END;
                    }else{
                        printf("Attemping to load level %d because posX: %f > width: %d\n", selectedLevel, playerPos.x, screenWidth);
                        if(loadNew(selectedLevel, false, levelImagePath, backgroundPath, &startingPos, &startingPos2, Col, levelText, crate, ladderCol, &colliderNum, &textNum, &ladderNum, &crateNum, &leverNum, &doorNum, &playerMode, &portalNum, wallTags, &wallNum, &wallEnabled, &goal, &scrollType, &leverFlip, levelPath) == 0){
                            printf("Succesfully loaded\n");
                        }else{
                            printf("Failed loading level.\n");
                            CloseWindow();
                            return 0;
                        }
                        prepareLevel(customLevel, selectedLevel, resolutionMultiplier, &playerPos, &playerPos2, &level, levelImagePath, &doorList, &leverList, wallNum, wallImg, &goal, leverFlip, levelPath);
                    }
                }else{
                    customLevel++;
                    printf("main: CUSTOM - Attemping to load level %d because posX: %f > width: %d\n", customLevel, playerPos.x, screenWidth);
                    if(loadNew(customLevel, true, levelImagePath, backgroundPath, &startingPos, &startingPos2, Col, levelText, crate, ladderCol, &colliderNum, &textNum, &ladderNum, &crateNum, &leverNum, &doorNum, &playerMode, &portalNum, wallTags, &wallNum, &wallEnabled, &goal, &scrollType, &leverFlip, levelPath) == 0){
                        printf("Succesfully loaded\n");
                    }else{
                        printf("Failed loading level.\n");
                        CloseWindow();
                        return 0;
                    }
                    prepareLevel(customLevel, selectedLevel, resolutionMultiplier, &playerPos, &playerPos2, &level, levelImagePath, &doorList, &leverList, wallNum, wallImg, &goal, leverFlip, levelPath);
                }
            }

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
            if(IsKeyDown(KEY_F8)){
                if(IsKeyPressed(KEY_UP)){
                    scrollType++;
                    if(scrollType > 4){
                        scrollType = 0;
                    }
                }
                if(IsKeyPressed(KEY_DOWN)){
                    scrollType--;
                    if(scrollType < 0){
                        scrollType = 4;
                    }
                }
            }

            //Camera Controller
            switch(scrollType){
                case 0:
                    camera.target = (Vector2){0,0};
                    camera.offset = (Vector2){0,0};
                    break;
                case 1:
                    camera.offset = (Vector2){0,0};
                    if(playerPos.x - (screenWidth / 2) < 0){
                        camera.target.x = 0;
                    }else if (playerPos.x - (level.width * resolutionMultiplier - screenWidth / 2) > 0){
                        camera.target.x = level.width * resolutionMultiplier - screenWidth;
                    }else{
                        camera.target.x = playerPos.x;
                        camera.offset.x = screenWidth / 2;
                    }
                    camera.target.y = 0;
                    break;
                case 2:
                    camera.offset = (Vector2){0,0};
                    if(playerPos.y - (screenHeight / 2) < 0){
                        camera.target.y = 0;
                    }else if (playerPos.y - (level.height * resolutionMultiplier - screenHeight / 2) > 0){
                        camera.target.y = level.height * resolutionMultiplier - screenHeight;
                    }
                    else{
                        camera.target.y = playerPos.y;
                        camera.offset.y = screenHeight / 2;
                    }
                    camera.target.x = 0;
                    break;
                case 3:
                    camera.offset = (Vector2){0,0};
                    if(playerPos.x - (screenWidth / 2) < 0){
                        camera.target.x = 0;
                    }else if (playerPos.x - (level.width * resolutionMultiplier - screenWidth / 2) > 0){
                        camera.target.x = level.width * resolutionMultiplier - screenWidth;
                    }else{
                        camera.target.x = playerPos.x;
                        camera.offset.x = screenWidth / 2;
                    }

                    if(playerPos.y - (screenHeight / 2) < 0){
                        camera.target.y = 0;
                    }else if (playerPos.y - (level.height * resolutionMultiplier - screenHeight / 2) > 0){
                        camera.target.y = level.height * resolutionMultiplier - screenHeight;
                    }
                    else{
                        camera.target.y = playerPos.y;
                        camera.offset.y = screenHeight / 2;
                    }
                    break;
                case 4:
                    camera.target = playerPos;
                    camera.offset = (Vector2){screenWidth / 2, screenHeight / 2};
                    break;
            }
        }
        
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(RAYWHITE);

            //Gameplay related textures
            if(gameState == STATE_ACTIVE || gameState == STATE_PAUSED){
                EndMode2D();
                //Background
                if(gameState == STATE_ACTIVE){
                    //active
                    DrawAnimationPro(&background, bgPosition, backgroundResMultiplier, WHITE, CYCLE_FORWARD);
                }else{
                    //paused
                    DrawAnimationPro(&background, bgPosition, backgroundResMultiplier, WHITE, CYCLE_NONE);
                }
                BeginMode2D(camera);

                //Level + Text
                DrawTextureEx(level, bgPosition, 0, resolutionMultiplier, WHITE);
                for(int i = 0; i < wallNum; i++){
                    //printf("Rendering wallImg[%d]\n", i);
                    if(((wallEnabled >> i) & 0b1) == 1)
                        DrawTextureEx(wallImg[i], bgPosition, 0, resolutionMultiplier, WHITE);
                    else
                        DrawTextureEx(wallImg[i], bgPosition, 0, resolutionMultiplier, GHOST);
                }
                for(int i = 0; i < textNum; i++){
                    DrawText(levelText[i].text, levelText[i].x, levelText[i].y, levelText[i].size, levelText[i].colour);
                }

                //Triangles

                //drawTriSlope(triCol[0], RED);

                //Levers
                for(int i = 0; i < leverNum; i++){
                    DrawTextureEx(leverList[i].frames[leverList[i].currentFrame], boxToVec2(Col[colliderNum + i]), 0, resolutionMultiplier, Col[colliderNum + i].enabled ? WHITE : GHOST);
                }

                //Doors
                for(int i = 0; i < doorNum; i++){
                    //give pointer directly
                    DrawAnimationPro(doorList+i, boxToVec2(Col[colliderNum + leverNum + i]), resolutionMultiplier, WHITE, CYCLE_NONE);
                }

                //Crates
                if(crateNum > 0){
                    for(int i = 0; i < crateNum; i++){
                        DrawTextureEx(crateImage, crate[i].position, 0, resolutionMultiplier, WHITE);
                    }
                }
                for(int i = colliderNum + leverNum + doorNum; i < colliderNum + leverNum + doorNum + portalNum; i++){
                    DrawAnimationPro((i % 2) == 0 ? &portal : &portal2, boxToVec2(Col[i]), resolutionMultiplier, WHITE, gameState == STATE_ACTIVE ? CYCLE_FORWARD : CYCLE_NONE);
                }

                //Ladders
                for(int i = 0; i < ladderNum; i++){
                    for(int step = 0; step < (ladderCol[i].sizeY / 3) / resolutionMultiplier; step++){
                        DrawTexturePro(ladder, (Rectangle){0,(step * 3) % 21,7,3}, (Rectangle){ladderCol[i].x, ladderCol[i].y + (step * 3 * resolutionMultiplier), 7 * resolutionMultiplier, 3 * resolutionMultiplier}, (Vector2){0,0}, 0, ladderCol[i].enabled ? WHITE : GHOST);
                    }
                    DrawTexturePro(ladder, (Rectangle){0,0,7,(ladderCol[i].sizeY % (3 * resolutionMultiplier)) / resolutionMultiplier}, (Rectangle){ladderCol[i].x, ladderCol[i].y + ladderCol[i].sizeY - (ladderCol[i].sizeY % (3 * resolutionMultiplier)), 7 * resolutionMultiplier, ladderCol[i].sizeY % (3 * resolutionMultiplier)}, (Vector2){0,0}, 0, ladderCol[i].enabled ? WHITE : GHOST);
                
                }

                //Player
                if(!(IsKeyDown(KEY_A) && IsKeyDown(KEY_D)) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) && gameState == STATE_ACTIVE && !disablePlayerAnim){
                    if(player_flipX){
                        DrawAnimationPro(&playerAnim_flipped, playerPos, resolutionMultiplier, WHITE, CYCLE_SHAKE);
                    }else{
                        DrawAnimationPro(&playerAnim, playerPos, resolutionMultiplier, WHITE, CYCLE_SHAKE);
                    }
                }else if(gameState == STATE_ACTIVE || inputVelocity.x == 0 || disablePlayerAnim){
                    if(player_flipX){
                        DrawTextureEx(player_flipped, playerPos, 0, resolutionMultiplier, WHITE);
                    }else{
                        DrawTextureEx(player, playerPos, 0, resolutionMultiplier, WHITE);
                    }
                }else{
                    if(inputVelocity.x < 0){
                        DrawAnimationPro(&playerAnim_flipped, playerPos, resolutionMultiplier, WHITE, CYCLE_NONE);
                    }else{
                        DrawAnimationPro(&playerAnim, playerPos, resolutionMultiplier, WHITE, CYCLE_NONE);
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
        
        EndMode2D();
        
            //Debug info
            if(gameState == STATE_ACTIVE){
                if(IsKeyDown(KEY_F8)){
                    DrawText(TextFormat("scrollType: %d", scrollType), 10, 10, 20, BLUE);
                }
                if(IsKeyDown(KEY_F6)){
                    DrawText("Manual Ladder Enabled", 10, 10, 50, WHITE);
                }
                if(IsKeyPressed(KEY_F7)){
                    for(int i = 0; i < colliderNum + leverNum + doorNum + crateNum + ladderNum + portalNum; i++){
                        printf("%d;", collision.colsTouched[i]);
                    }
                    printf("\n");
                }
                BeginMode2D(camera);
                    //Debug related 
                    if(ColliderDebugMode){
                        if(IsKeyDown(KEY_E)){
                            DrawRectangleRec((Rectangle){
                            playerPos.x - (resolutionMultiplier * 2),
                            playerPos.y + resolutionMultiplier,
                            playerSize.x + (resolutionMultiplier * 4),
                            playerSize.y - (resolutionMultiplier * 2)}, MAGENTA);
                        }
                        if(!(IsKeyDown(KEY_A) && IsKeyDown(KEY_D)) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) && !disablePlayerAnim) DrawRectangle(playerPos.x, playerPos.y, playerSizeLarge.x, playerSizeLarge.y, PINK);
                            else DrawRectangle(playerPos.x, playerPos.y, playerSize.x, playerSize.y, PINK);
                        //if(triCollision.botRight) DrawCircleV(addVec2(playerPos, playerSize), 3, PURPLE);
                        //if(triCollision.topLeft) DrawCircleV(playerPos, 3, PURPLE);
                        //if(triCollision.topRight) DrawCircle(playerPos.x + playerSize.x, playerPos.y, 3, PURPLE);
                        //if(triCollision.botLeft) DrawCircle(playerPos.x, playerPos.y + playerSize.y, 3, PURPLE);
                        int colsToResize = colliderNum + leverNum + doorNum + portalNum;
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
                        for(int i = 0; i < crateNum; i++){
                            DrawRectangle(crate[i].position.x, crate[i].position.y, crate[i].sizeX, crate[i].sizeY, MAROON);
                        }
                        //DrawCircle(playerPos.x + (playerSize.x / 2),triCollision.floor + playerSize.y, 3, PURPLE);
                        //DrawCircle(playerPos2.x + (playerSize.x / 2),triCollision2.floor + playerSize.y, 3, PURPLE);
                        //drawSlope(triCol[0].slope, triCol[0].init);
                        DrawRectangle(goal.x, goal.y, goal.sizeX, goal.sizeY, DARKGREEN);
                        DrawRectangleLines(goal.x, goal.y, goal.sizeX, goal.sizeY, BLACK);
                    }
                    if(ShowCollider > 0){
                        DrawRectangle(Col[ShowCollider - 1].x, Col[ShowCollider - 1].y, Col[ShowCollider - 1].sizeX, Col[ShowCollider - 1].sizeY, GREEN);
                    }
                EndMode2D();
                    if(ShowCollider > 0){
                        sprintf(str, "%f", playerPos.y + playerSize.y);
                        DrawText(str, 0, 0, 100, GREEN);
                        sprintf(str, "%d", Col[ShowCollider - 1].y);
                        DrawText(str, 0, 100, 100, GREEN);
                        sprintf(str, "%d", Col[ShowCollider - 1].y + Col[ShowCollider - 1].sizeY);
                        DrawText(str, 0, 200, 100, GREEN);
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
                renderGuiImg(pauseIcon, WHITE);

                if(IsMouseButtonDown(0) && isMouseInGuiBox(pauseButton)){
                    gameState = STATE_PAUSED;
                }
                

            }

            //Menu GUI
            if(gameState == STATE_MENU){

                //background
                DrawTextureEx(main_menu, bgPosition, 0, resolutionMultiplier, WHITE);

                //title
                renderGuiText(titleText);
                renderGuiText(titleText2);


                if(levelSelectVisibilityMode != 0){
                    backText = setGuiTextOrigin(backButton, backText, true);
                    renderGuiBox(backButton, true);

                    if(IsMouseButtonPressed(0) && isMouseInGuiBox(nextButton)){
                        levelSelectVisibilityMode = levelSelectVisibilityMode == 1 ? 2 : 1;
                        nextButton.color = LIGHTGRAY;
                    }else if(!IsMouseButtonDown(0) || !isMouseInGuiBox(nextButton)){
                        nextButton.color = WHITE;
                    }

                    nextText = setGuiTextOrigin(nextButton, nextText, true);
                    renderGuiBox(nextButton, true);

                    if(IsMouseButtonPressed(0) && isMouseInGuiBox(backButton)){
                        levelSelectVisibilityMode = 0;
                    }

                    for(int i = (levelSelectVisibilityMode == 1 ? 0 : 6); i < (levelSelectVisibilityMode == 1 ? 6 : 12); i++){
                        //printf("lSVM: %d   i: %d\n", levelSelectVisibilityMode, i);
                        levelSelectText[i] = setGuiTextOrigin(levelSelect[i], levelSelectText[i], true);
                        renderGuiBox(levelSelect[i], true);

                        if(IsMouseButtonPressed(0) && isMouseInGuiBox(levelSelect[i])){
                            selectedLevel = i;
                            customLevel = 0;
                            loadNew(selectedLevel, false, levelImagePath, backgroundPath, &startingPos, &startingPos2, Col, levelText, crate, ladderCol, &colliderNum, &textNum, &ladderNum, &crateNum, &leverNum, &doorNum, &playerMode, &portalNum, wallTags, &wallNum, &wallEnabled, &goal, &scrollType, &leverFlip, levelPath);
                            prepareLevel(customLevel, selectedLevel, resolutionMultiplier, &playerPos, &playerPos2, &level, levelImagePath, &doorList, &leverList, wallNum, wallImg, &goal, leverFlip, levelPath);
                            gameState = STATE_ACTIVE;
                            levelSelectVisibilityMode = 0;
                            break;
                        }
                    }
                }

                if(levelSelectVisibilityMode == 0 && gameState == STATE_MENU){
                    playText = setGuiTextOrigin(playButton, playText, true);
                    renderGuiBox(playButton, true);

                    selectText = setGuiTextOrigin(selectButton, selectText, true);
                    selectText2 = setGuiTextOrigin(selectButton, selectText2, true);
                    renderGuiBox(selectButton, true);

                    if(IsMouseButtonPressed(0) && isMouseInGuiBox(playButton)){
                        gameState = STATE_CUTSCENE;
                        timer = 3.0f; // timer before cutscene changes frames
                        levelSelectVisibilityMode = 0;
                        
                    }

                    if(IsMouseButtonPressed(0) && isMouseInGuiBox(selectButton)){
                        levelSelectVisibilityMode = 1;
                    }
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

            if(gameState == STATE_END){
                DrawAnimationPro(&background, bgPosition, backgroundResMultiplier, WHITE, CYCLE_NONE);
                DrawTextureEx(end_screen, (Vector2){0,0}, 0, resolutionMultiplier, WHITE);
            }

            if(gameState == STATE_CUTSCENE){
                //background
                //DrawTextureEx(main_menu, bgPosition, 0, resolutionMultiplier, WHITE);
                ClearBackground(BLACK);
                timer -= GetFrameTime();
                if(timer < 0.0f){
                    DrawAnimationPro(&cutscene, VEC2ZERO, resolutionMultiplier, GRAY, CYCLE_NONE);
                    cutscene.currentFrame++;
                    if(cutscene.currentFrame > cutscene.frameCount - 1){
                        cutscene.isAnimating = false;
                    }
                    timer = 1.5f;
                }else if(timer < 1.0f){
                    DrawAnimationPro(&cutscene, VEC2ZERO, resolutionMultiplier, GRAY, CYCLE_NONE);
                }
                
                if(anyButtonPrompt){
                    renderGuiText(promptText);
                    if(IsKeyPressed(KEY_SPACE)){
                        cutscene.isAnimating = false;
                    }
                }else{
                    if(GetKeyPressed() != 0){
                        anyButtonPrompt = true;
                    }
                }

                if(!cutscene.isAnimating){
                    gameState = STATE_ACTIVE;
                    customLevel = 0;
                    selectedLevel = 0;
                    loadNew(selectedLevel, false, levelImagePath, backgroundPath, &startingPos, &startingPos2, Col, levelText, crate, ladderCol, &colliderNum, &textNum, &ladderNum, &crateNum, &leverNum, &doorNum, &playerMode, &portalNum, wallTags, &wallNum, &wallEnabled, &goal, &scrollType, &leverFlip, levelPath);
                    prepareLevel(customLevel, selectedLevel, resolutionMultiplier, &playerPos, &playerPos2, &level, levelImagePath, &doorList, &leverList, wallNum, wallImg, &goal, leverFlip, levelPath);
                }
            }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void prepareLevel(int customLevel, int selectedLevel, int resolutionMultiplier,
                    Vector2* playerPos, Vector2* playerPos2,
                    Texture2D* level, char levelImagePath[64],
                    Animation** doorList, SwitchAnimation** leverList,
                    int wallNum, Texture2D wallImg[16], BoxCollider2D* goal, uint64_t leverFlip, char levelPath[128]

                    //bool** door_isClosedList,
                    //char* levelWallsImgPath, int* pathLength, 
                    //Texture* defaultDoor, Texture* leverOn, Texture* leverOff
                    //Animation* background, char backgroundPath[128]
                    ){
    printf("prepareLevel: Preparing level...\n");
    //load level image
    char str[256];
    if(customLevel == 0){
        sprintf(str, "resources/levels/%d.png", selectedLevel + 1);
        printf("prepareLevel: Attemping to load image: %s\n", str);
        *level = LoadTexture(str);
        for(int i = 0; i < wallNum; i++){
            wallImg[i] = LoadTexture(TextFormat("resources/levels/%d_walls/%d.png", selectedLevel + 1, i));
        }
        //if(!TextIsEqual("@", backgroundPath)) background->texture = LoadTexture(TextFormat("resources/levels/%s", backgroundPath));
    }else{
        printf("prepareLevel: customLevel enabled. Attemping to load image from levelImagePath. If the program crashes and no other messages pop up, make sure you've specified the img property\n");
        if(levelPath[TextLength(levelPath) - 1] == '/'){
            sprintf(str, "custom_levels/%s%d.png", levelPath, customLevel);
            for(int i = 0; i < wallNum; i++){
                wallImg[i] = LoadTexture(TextFormat("custom_levels/%s%d_walls/%d.png", levelPath, customLevel, i));
            }
        }else{
            sprintf(str, "custom_levels/%s", levelImagePath);
            for(int i = 0; i < wallNum; i++){
                wallImg[i] = LoadTexture(TextFormat("custom_levels/%s_walls/%d.png", levelPath, i));
            }
        }
        printf("prepareLevel: Attemping to load custom image: %s\n", str);
        *level = LoadTexture(str);
        //if(!TextIsEqual("@", backgroundPath)) background->texture = LoadTexture(TextFormat("custom_levels/%s", backgroundPath));
    }
    


    //Resize doors + levers lists
    //Uses double-pointers so that we reference the locations of the actual arrays and not the ones given to the function
    printf("prepareLevel: Resizing lists...\n");
    printf("prepareLevel: doorList size: %I64ld\n", sizeof(Animation) * doorNum);
    *doorList = (Animation*)realloc(*doorList, sizeof(Animation) * doorNum);

    //printf("prepareLevel: door_isClosedList size: %I64d\n", sizeof(bool) * doorNum);
    //*door_isClosedList = (bool*)realloc(*door_isClosedList, sizeof(bool) * doorNum);

    printf("prepareLevel: Setting default values for new lists...\n");
    for(int i = 0; i < doorNum; i++){
        printf("prepareLevel: prepareLevel: test1: %p\n", *doorList);
        (*doorList)[i] = (Animation){0,0,0,0,0,0,0,0,0,0,0,0,0};
        printf("prepareLevel: hasn't crashed\n");
        (*doorList)[i] = assignProperties(0, 0, 60, false, 12, false);

        (*doorList)[i] = getFromFolder((*doorList)[i], TextFormat("resources/objects/door%d/", (Col[i + colliderNum  + leverNum].tag - 1) % 6), true);
        printf("loading: tag = %d, mod6 = %d, x = %d id = %d\n", Col[i + colliderNum  + leverNum].tag - 1, (Col[i + colliderNum + leverNum].tag - 1) % 6, Col[i + colliderNum  + leverNum].x, i + colliderNum  + leverNum);
        //(*doorList)[i].texture = *defaultDoor;

        //(*door_isClosedList)[i] = true;
        printf("prepareLevel: default vals - Completed obj %d\n", i);
    }

    printf("prepareLevel: leverList size: %I64ld\n", sizeof(SwitchAnimation) * leverNum);
    *leverList = (SwitchAnimation*)realloc(*leverList, sizeof(SwitchAnimation) * leverNum);
    for(int i = 0; i < leverNum; i++){
        (*leverList)[i] = switchAssignProperties(0, 10, false);

        (*leverList)[i] = switchGetFromFolder((*leverList)[i], TextFormat("resources/objects/lever%d/", (Col[i + colliderNum].trigger - 1) % 6));
        if(((leverFlip >> i) & 0b1) == 1)(*leverList)[i] = flipSwitchAnimationHorizontal((*leverList)[i]);
        printf("loading: trigger = %d, mod6 = %d, x = %d, id = %d\n", Col[i + colliderNum].trigger - 1, (Col[i + colliderNum].trigger - 1) % 6, Col[i + colliderNum].x, i + colliderNum);
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
        printf("Collider[%d].x + .sizeX = %d\n",i, Col[i].x + Col[i].sizeX);
    }

    // resize goal

    goal->x *= resolutionMultiplier;
    goal->y *= resolutionMultiplier;
    goal->sizeX *= resolutionMultiplier;
    goal->sizeY *= resolutionMultiplier;

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
    startingPos.x *= resolutionMultiplier;
    startingPos.y *= resolutionMultiplier;
    playerPos->x = startingPos.x;
    playerPos->y = startingPos.y;

    startingPos2.x *= resolutionMultiplier;
    startingPos2.y *= resolutionMultiplier;
    playerPos2->x = startingPos2.x;
    playerPos2->y = startingPos2.y;

    for(int i = 0; i < crateNum; i++){
        crate[i].position.x *= resolutionMultiplier;
        crate[i].position.y *= resolutionMultiplier;
        crate[i].sizeX *= resolutionMultiplier;
        crate[i].sizeY *= resolutionMultiplier;
        crate[i].velocity.x = 0;
        crate[i].velocity.y = 0;
    }
    printf("Level prepared succesfully. Starting level...\n");
}
    