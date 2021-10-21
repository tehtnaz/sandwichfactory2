#ifndef GUI_H_2
    #define GUI_H_2
    
    #include "raylib.h"
    #include "vector2pp.h"
    #include "recpp.h"

    typedef struct GuiText{
        Vector2 center;
        Vector2 offset;

        char text[50];

        Color color;
        int size;
        float spacing;
    }GuiText;

    typedef struct GuiImg{
        Texture2D img;
        Vector2 position;
        Vector2 center;
    }GuiImg;

    typedef struct GuiBox{
        Rectangle rec;
        GuiText* texts[2];
        int textNum;

        Color color;
        int borderWidth;
        Color borderColor;
    }GuiBox;

    GuiBox assignGuiBox(int borderWidth, Rectangle rec, Color color, Color borderColor, GuiText* texts[2], int textNum){
        GuiBox temp;
        temp.borderWidth = borderWidth;
        temp.rec = rec;
        temp.color = color;
        temp.borderColor = borderColor;
        temp.texts[0] = texts[0];
        temp.texts[1] = texts[1];
        temp.textNum = textNum;
        return temp;
    }

    GuiText assignGuiText(Vector2 center, Vector2 offset, char text[50], int size, Color color, float spacing){
        GuiText temp;
        temp.center = center;
        temp.offset = offset;
        TextCopy(temp.text, text);
        temp.size = size;
        temp.color = color;
        temp.spacing = spacing;
        return temp;
    }

    void renderGuiText(GuiText input){
        DrawTextEx(GetFontDefault(), input.text, addVec2(input.center, input.offset), input.size, input.spacing, input.color);
    }

    void renderGuiBox(GuiBox input, bool renderText){
        if(input.borderWidth == 0){
            DrawRectangleRec(input.rec, input.color);
        }else{
            DrawRectangleRec(input.rec, input.color);
            DrawRectangleLinesEx(input.rec, input.borderWidth, input.borderColor);
        }

        /*if(renderText){
            if(input.textNum > 0){
                for(int i = 0; i < input.textNum; i++){
                    renderGuiText(*(input.texts[i]));
                }
            }else{
                printf("Warning: renderGuiBox - Tried to render text when textNum is 0");
            }
        }*/
    }

    bool isMouseInGuiBox(GuiBox box){
        return isPointInRec(box.rec, (Vector2){GetMouseX(), GetMouseY()});
    }


#endif // gui_h_2