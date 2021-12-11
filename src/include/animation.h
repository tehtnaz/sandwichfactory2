#ifndef ANIMATION_H_2
    #define ANIMATION_H_2
    #include "raylib.h"

// TODO:
// Maybe split all the sections of the main.c loop into different functions
//try to fix prepare level
//with all source files containing texutre loading, add texture unloading maybe? (ie. UnloadAnimation)



//Animation.h version 2.0

// Changes from version 1 (present in sandwich factory)
    // allowSnap has new definition (defined on line 30)
    // warns you when attempting to cycle an animation that is not ".isAnimating"

//1.2 changelog
    //added shakeCycleAnimation

//2.0 changelog
    //added drawAnimation
    //BIG: changed frame array to texture atlas
    //shakeCycleAnimation now relies on negative FPS. Try not to use negative FPS while doing regular anims
    //WATCH OUT: cycleAnimation and cycleAnimationBackwards will still not accept negative FPS!


typedef struct Animation{
    Texture2D texture; // sprite sheet
    int spriteSize; //width, since atlas height is defualt height of sprite which also means you can't stack vertically
    
    int frameCount; // Defines where we stop reading data  (eg. 0 to 5  = 6 items)
    int fps; //Instead of the rendered fps, use whatever you like! Only accepts negative fps if doing shakeCycle (still not recommended)
    int currentFrame;
    bool isAnimating;
    bool allowSnap; // This defines whether the currentFrame will reset to 0 once it overflows, or whether it won't. Vice versa for underflowing. If false, it will stop the animation.
    bool cycleBackward;
    
    float frameBuffer; //Amount of frames since last switch. This is scaled to the rendered FPS so that this animation can have any speed you want
}Animation;

typedef struct SwitchAnimation{
    Texture2D frames[2];
    int fps;
    bool isAnimating;
    int currentFrame;
}SwitchAnimation;

typedef enum CycleType{
    CYCLE_NONE,
    CYCLE_FORWARD,
    CYCLE_BACKWARD,
    CYCLE_SHAKE
}CycleType;

Animation assignProperties(int spriteSize, int currentFrame, int fps, bool isAnimating, int frameCount, bool allowsnap);
SwitchAnimation switchAssignProperties(int currentFrame, int fps, bool isAnimation);

Animation getFromFolder(Animation input, const char path[64], bool autoSize);
SwitchAnimation switchGetFromFolder(SwitchAnimation input, const char path[64]);

Texture getTextureFromFolder(const char path[64], int textureCount);

Animation cycleAnimation(Animation input, float screenFPS);
Animation cycleAnimationBackwards(Animation input, float screenFPS);
Animation shakeCycleAnimation(Animation input, float screenFPS); //ignores isAnimating property and only works while allowSnap property is disabled

void DrawAnimationPro(Animation* input, Vector2 position, float scale, Color tint, float screenFPS, CycleType cycleAnim);

Animation flipAnimationHorizontal(Animation input);
SwitchAnimation flipSwitchAnimationHorizontal(SwitchAnimation input);

#endif //animation_h_2