#include "raylib.h"
#include <stdio.h>
#include "animation.h"
#include "vector2pp.h"

// Animation.h version 2.0

Animation assignProperties(int spriteSize, int currentFrame, int fps, bool isAnimating, int frameCount, bool allowsnap){
    Animation temp;

    temp.spriteSize = spriteSize;

    temp.frameCount = frameCount;
    temp.fps = fps;
    temp.currentFrame = currentFrame;
    temp.isAnimating = isAnimating;
    temp.allowSnap = allowsnap;
    temp.cycleBackward = false;

    temp.frameBuffer = 0;
    return temp;
}

SwitchAnimation switchAssignProperties(int currentFrame, int fps, bool isAnimation){
    SwitchAnimation temp;
    temp.fps = fps;
    temp.isAnimating = isAnimation;
    temp.currentFrame = currentFrame;
    return temp;
}



Animation getFromFolder(Animation input, const char path[64], bool autoSize){
    Animation temp = input;
    char str[70];
    sprintf(str, "%s0.png", path);
    printf("getFromFolder: Base Image = %s\n", str);
    Image img = LoadImage(str);
    if(autoSize){
        temp.spriteSize = img.width;
        printf("getFromFolder: animation spriteSize (AUTO): %d\n", temp.spriteSize);
    }else{
        printf("getFromFolder: animation spriteSize: %d\n", temp.spriteSize);
    }
    Image atlas = GenImageColor(img.width * input.frameCount, img.height, BLANK);
    for(int i = 0; i < input.frameCount; i++){
        sprintf(str, "%s%d.png", path, i);
        img = LoadImage(str);
        ImageDraw(&atlas, img, (Rectangle){0,0,img.width,img.height}, (Rectangle){temp.spriteSize * i,0,img.width,img.height}, WHITE);
    }
    UnloadImage(img);
    temp.texture = LoadTextureFromImage(atlas);
    UnloadImage(atlas);
    return temp;
}

Texture getTextureFromFolder(const char path[64], int textureCount){
    char str[70];
    sprintf(str, "%s0.png", path);
    printf("getFromFolder: Base Image = %s\n", str);
    Image img = LoadImage(str);
    Image atlas = GenImageColor(img.width * textureCount, img.height, BLANK);
    for(int i = 0; i < textureCount; i++){
        sprintf(str, "%s%d.png", path, i);
        img = LoadImage(str);
        ImageDraw(&atlas, img, (Rectangle){0,0,img.width,img.height}, (Rectangle){img.width * i,0,img.width,img.height}, WHITE);
    }
    Texture tempTexture = LoadTextureFromImage(atlas);
    UnloadImage(img);
    UnloadImage(atlas);
    return tempTexture;
}

SwitchAnimation switchGetFromFolder(SwitchAnimation input, const char path[64]){
    SwitchAnimation temp = input;
    char str[70];
    sprintf(str, "%s0.png", path);
    temp.frames[0] = LoadTexture(str);
    sprintf(str, "%s1.png", path);
    temp.frames[1] = LoadTexture(str);
    return temp;
}



Animation cycleAnimation(Animation input, float screenFPS){
    if(input.isAnimating == false){
        printf("Warning: cycleAnimation - Attempted to cycle an animation which was disabled.\n");
        return input;
    }
    if(input.fps < 0){
        printf("ERROR: cycleAnimation - Attempted to cycle with negative fps");
        return input;
    }
    Animation temp = input;

    temp.frameBuffer += temp.fps / screenFPS;
    
    while(temp.frameBuffer >= 1){
            temp.frameBuffer--;
            temp.currentFrame++;
    }

    if(temp.currentFrame > temp.frameCount - 1 && temp.allowSnap){
        temp.currentFrame = 0;
    }
    if(temp.currentFrame >= temp.frameCount - 1 && !temp.allowSnap){
        temp.currentFrame = temp.frameCount - 1;
        temp.isAnimating = false;
    }
    return temp;
}

Animation cycleAnimationBackwards(Animation input, float screenFPS){
    if(input.isAnimating == false){
        printf("Warning: cycleAnimationBackwards - Attempted to cycle an animation which was disabled.\n");
        return input;
    }
    if(input.fps < 0){
        printf("ERROR: cycleAnimationBackwards - Attempted to cycleBackwards with negative fps");
        return input;
    }
    Animation temp = input;

    temp.frameBuffer += temp.fps / screenFPS;

    while(temp.frameBuffer >= 1){
            temp.frameBuffer--;
            temp.currentFrame--;
    }

    if(temp.currentFrame < 0 && temp.allowSnap){
        temp.currentFrame = temp.frameCount - 1;
    }
    if(temp.currentFrame <= 0 && !temp.allowSnap){
        temp.currentFrame = 0;
        temp.isAnimating = false;
    }
    return temp;
}

//ignores isAnimating property and only works while allowSnap property is disabled

Animation shakeCycleAnimation(Animation input, float screenFPS){
    if(input.allowSnap){
        printf("allowSnap is enabled for this object. Did you mean to cycle it and not shake it? Skipping...\n");
        return input;
    }
    if(input.isAnimating){
        if(!input.cycleBackward){
            //printf("cycle\n");
            return cycleAnimation(input, screenFPS);
        }else{
            //printf("cycleBack\n");
            return cycleAnimationBackwards(input, screenFPS);
        }
    }else{
        Animation temp = input;
        temp.cycleBackward = !temp.cycleBackward;
        temp.isAnimating = true;
        if(!temp.cycleBackward){
            //printf("cycle+switch\n");
            return cycleAnimation(temp, screenFPS);
        }else{
            //printf("cycleBack+switch\n");
            return cycleAnimationBackwards(temp, screenFPS);
        }
    }
}


/*
* 0 = Do not cycle
* 1 = cycle
* 2 = cycleBackward
* 3 = shakeCycle
*/



void DrawAnimationPro(Animation* input, Vector2 position, float scale, Color tint, float screenFPS, CycleType cycleAnim){
    DrawTextureTiled(input->texture,
                    (Rectangle){input->currentFrame * input->spriteSize, 0, input->spriteSize, input->texture.height},
                    (Rectangle){0,0,scale*input->spriteSize,scale*input->texture.height},
                    negVec2(position),0,scale,tint);
    switch(cycleAnim){
        case CYCLE_NONE:
            break;
        case CYCLE_FORWARD:
            *input = cycleAnimation((*input), screenFPS);
            break;
        case CYCLE_BACKWARD:
            *input = cycleAnimationBackwards((*input), screenFPS);
            break;
        case CYCLE_SHAKE:
            *input = shakeCycleAnimation((*input), screenFPS);
            break;
    }
}



Animation flipAnimationHorizontal(Animation input){
    Animation temp = input;
    Image animTexture = LoadImageFromTexture(temp.texture);
    Image tempImg = GenImageColor(temp.spriteSize, temp.texture.height, WHITE);
    for(int i = 0; i < temp.frameCount; i++){
        ImageDraw(&tempImg, animTexture, (Rectangle){temp.frameCount * temp.spriteSize, 0, temp.spriteSize, temp.texture.height}, (Rectangle){0,0,temp.spriteSize, temp.texture.height}, WHITE);
        ImageFlipHorizontal(&tempImg);
        ImageDraw(&animTexture, tempImg, (Rectangle){0,0,temp.spriteSize, temp.texture.height}, (Rectangle){temp.frameCount * temp.spriteSize, 0, temp.spriteSize, temp.texture.height}, WHITE);
    }
    UnloadTexture(temp.texture);
    temp.texture = LoadTextureFromImage(tempImg);
    UnloadImage(tempImg);
    UnloadImage(animTexture);
    return temp;
}

SwitchAnimation flipSwitchAnimationHorizontal(SwitchAnimation input){
    SwitchAnimation temp = input;
    Image tempImg0 = LoadImageFromTexture(temp.frames[0]);
    Image tempImg1 = LoadImageFromTexture(temp.frames[1]);
    UnloadTexture(temp.frames[0]);
    UnloadTexture(temp.frames[1]);

    ImageFlipHorizontal(&tempImg0);
    ImageFlipHorizontal(&tempImg1);

    temp.frames[0] = LoadTextureFromImage(tempImg0);
    temp.frames[1] = LoadTextureFromImage(tempImg1);
    UnloadImage(tempImg0);
    UnloadImage(tempImg1);
    return temp;
}