#ifndef GUI_H_2
    #define GUI_H_2
    
    #include "raylib.h"
    #include "vector2pp.h"
    #include "recpp.h"

    typedef struct GuiText{
        Font* font;
        Vector2 center; // default = (0, 0)   can be used to make a GuiBox a parent object
        Vector2 offset;

        char text[50];

        Color color;
        int size;
        float spacing;
    }GuiText;

    typedef struct GuiImg{
        Texture2D img;
        Vector2 center;
        Vector2 offset;
        float scale;
    }GuiImg;

    typedef struct GuiBox{
        Rectangle rec;
        GuiText* texts[2];
        int textNum;

        Color color;
        int borderWidth;
        Color borderColor;
    }GuiBox;

    GuiBox assignGuiBox(Rectangle rec, GuiText* text1, GuiText* text2, int textNum, Color color, int borderWidth, Color borderColor){
        GuiBox temp;
        temp.rec = rec;
        temp.texts[0] = text1;
        temp.texts[1] = text2;
        temp.textNum = textNum;

        temp.color = color;
        temp.borderWidth = borderWidth;
        temp.borderColor = borderColor;
        
        return temp;
    }

    GuiImg assignGuiImg(Texture2D img, Vector2 center, Vector2 offset, float scale){
        GuiImg temp;
        temp.img = img;
        temp.center = center;
        temp.offset = offset;
        temp.scale = scale;
        return temp;
    }

    //if fromScreenCenter, offsets center of sprite from center of screen
    //else offset from center of sprite
    GuiBox offsetGuiBox(GuiBox guiBox, Vector2 position, Vector2 size, bool fromScreenCenter, int screenWidth, int screenHeight){
        GuiBox temp = guiBox;
        if(fromScreenCenter)  temp.rec = combineVec2((Vector2){position.x - size.x/2 + screenWidth/2, position.y - size.y/2 + screenHeight/2}, size);
        else temp.rec = combineVec2((Vector2){position.x-size.x/2, position.y-size.y/2}, size);
        return temp;
    }

    GuiText assignGuiText(Font* font, Vector2 center, Vector2 offset, const char text[50], int size, Color color, float spacing){
        GuiText temp;
        temp.font = font;
        temp.center = center;
        temp.offset = offset;
        TextCopy(temp.text, text);
        temp.size = size;
        temp.color = color;
        temp.spacing = spacing;
        return temp;
    }

    void renderGuiImg(GuiImg input, Color tint){
        DrawTextureEx(input.img, addVec2(input.center, input.offset), 0, input.scale, tint);
    }

    void renderGuiText(GuiText input){
        DrawTextEx(*input.font, input.text, addVec2(input.center, input.offset), input.size, input.spacing, input.color);
    }

    void renderGuiBox(GuiBox input, bool renderText){
        if(input.borderWidth == 0){
            DrawRectangleRec(input.rec, input.color);
        }else{
            DrawRectangleRec(input.rec, input.color);
            DrawRectangleLinesEx(input.rec, input.borderWidth, input.borderColor);
        }

        if(renderText){
            if(input.textNum > 0){
                for(int i = 0; i < input.textNum; i++){
                    renderGuiText(*(input.texts[i]));
                }
            }else{
                printf("Warning: renderGuiBox - Tried to render text when textNum is 0");
            }
        }
    }

    GuiText setGuiTextOrigin(GuiBox box, GuiText guiText, bool fromBoxCenter){
        GuiText temp = guiText;
        if(fromBoxCenter){
            Vector2 textSize = MeasureTextEx(*guiText.font, guiText.text, guiText.size, guiText.spacing);
            //Vector2 textSize = MeasureTextEx(GetFontDefault(), guiText.text, guiText.size, guiText.spacing);
            temp.center = (Vector2){box.rec.x + box.rec.width / 2 - textSize.x / 2, box.rec.y + box.rec.height / 2 - textSize.y / 2};
        }else{
            temp.center = (Vector2){box.rec.x, box.rec.y};
        }
        return temp;
    }

    Vector2 GetTextCenter(Font font, const char text[64], float fontSize, float spacing, int screenWidth, int screenHeight){
        Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);
        return (Vector2){screenWidth / 2 - textSize.x / 2, screenHeight / 2 - textSize.y / 2};
    }

    Vector2 GetTextCenterGUI(GuiText guiText, int screenWidth, int screenHeight){
        return GetTextCenter(*guiText.font, guiText.text, guiText.size, guiText.spacing, screenWidth, screenHeight);
        //return GetTextCenter(GetFontDefault(), guiText.text, guiText.size, guiText.spacing, screenWidth, screenHeight);
    }

    bool isMouseInGuiBox(GuiBox box){
        return isPointInRec(box.rec, (Vector2){GetMouseX(), GetMouseY()});
    }

    GuiText resizeGuiText(GuiText guiText, float resizeAmount){
        GuiText temp = guiText;
        temp.center.x *= resizeAmount;
        temp.center.y *= resizeAmount;
        temp.offset.x *= resizeAmount;
        temp.offset.y *= resizeAmount;
        temp.size *= resizeAmount;
        temp.spacing *= resizeAmount;
        //if(!resizeCenter) 
        printf("before resizeGuiText: %f %f %f %f %d %f\n", guiText.center.x, guiText.center.y, guiText.offset.x, guiText.offset.y, guiText.size, guiText.spacing);
        //if(!resizeCenter) 
        printf("resizeGuiText: %f %f %f %f %d %f\n", temp.center.x, temp.center.y, temp.offset.x, temp.offset.y, temp.size, temp.spacing);
        return temp;
    }
    GuiBox resizeGuiBox(GuiBox guiBox, float resizeAmount, bool resizeTexts){
        GuiBox temp = guiBox;
        temp.rec = resizeRec(temp.rec, resizeAmount);
        temp.borderWidth *= resizeAmount;

        printf("before resizeGuiBox: %f %f %f %f %d\n", guiBox.rec.x, guiBox.rec.y, guiBox.rec.width, guiBox.rec.height, guiBox.borderWidth);
        printf("resizeGuiBox: %f %f %f %f %d\n", temp.rec.x, temp.rec.y, temp.rec.width, temp.rec.height, temp.borderWidth);

        if(resizeTexts){
            for(int i = 0; i < temp.textNum; i++){
                printf("-> ;  ");
                *(temp.texts[i]) = resizeGuiText(*(temp.texts[i]), resizeAmount);
            }
        }
        return temp;
    }
    GuiImg resizeGuiImg(GuiImg guiImg, float resizeAmount){
        GuiImg temp = guiImg;
        temp.scale *= resizeAmount;
        temp.offset.x *= resizeAmount;
        temp.offset.y *= resizeAmount;
        temp.center.x *= resizeAmount;
        temp.center.y *= resizeAmount;
        return temp;
    }

#endif // gui_h_2