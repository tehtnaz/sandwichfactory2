#include "raylib.h"
#include "vector2pp.h"
#include "recpp.h"
#include "gui.h"
#pragma once

void renderGuiObject(GuiObject input){
    if(input.hidden) return;

    if(input.box.borderWidth == 0){
        DrawRectangleRec(addVec2ToRec(input.box.rect, input.offset), input.box.color);
    }else{
        DrawRectangleRec(addVec2ToRec(input.box.rect, input.offset), input.box.color);
        DrawRectangleLinesEx(addVec2ToRec(input.box.rect, input.offset), input.box.borderWidth, BLACK);
    }

    if(input.textBox.wrap == true){
        DrawTextRec(GetFontDefault(), input.textBox.text, addVec2ToRec(input.box.rect, input.offset), input.textBox.size, input.textBox.spacing, true, input.textBox.color);
    }else{
        DrawTextEx(GetFontDefault(), input.textBox.text, addVec2(input.textBox.position, input.offset), input.textBox.size, input.textBox.spacing, input.textBox.color);
    }
}

void renderGuiInteract(GuiInteract input){
    if(input.hidden) return;

    if(input.button.borderWidth == 0){
        DrawRectangleRec(addVec2ToRec(input.button.rect, input.offset), input.button.color);
    }else{
        DrawRectangleRec(addVec2ToRec(input.button.rect, input.offset), input.button.color);
        DrawRectangleLinesEx(addVec2ToRec(input.button.rect, input.offset), input.button.borderWidth, BLACK);
    }

    if(input.textBox.wrap == true){
        DrawTextRec(GetFontDefault(), input.textBox.text, addVec2ToRec(input.button.rect, input.offset), input.textBox.size, input.textBox.spacing, true, input.textBox.color);
    }else{
        DrawTextEx(GetFontDefault(), input.textBox.text, addVec2(input.textBox.position, input.offset), input.textBox.size, input.textBox.spacing, input.textBox.color);
    }
}

void renderGuiText(GuiText input){
    DrawTextEx(GetFontDefault(), input.text, input.position, input.size, input.spacing, input.color);
}

void renderGuiBox(GuiBox input){
    if(input.borderWidth == 0){
        DrawRectangleRec(input.rect, input.color);
    }else{
        DrawRectangleRec(input.rect, input.color);
        DrawRectangleLinesEx(input.rect, input.borderWidth, BLACK);
    }
}

void renderGuiButton(GuiButton input){
    if(input.borderWidth == 0){
        DrawRectangleRec(input.rect, input.color);
    }else{
        DrawRectangleRec(input.rect, input.color);
        DrawRectangleLinesEx(input.rect, input.borderWidth, BLACK);
    }
}

GuiBox assignGuiBox(int borderWidth, Rectangle rect, Color color){
    GuiBox temp;
    temp.borderWidth = borderWidth;
    temp.rect = rect;
    temp.color = color;
    //temp.borderColor = borderColor;
    return temp;
}

GuiButton assignGuiButton(int borderWidth, Rectangle rect, Color color){
    GuiButton temp;
    temp.borderWidth = borderWidth;
    temp.rect = rect;
    temp.color = color;
    //temp.borderColor = borderColor;
    return temp;
}

GuiText assignGuiText(Vector2 position, char text[50], int size, Color color, bool wrap, float spacing){
    GuiText temp;
    temp.position = position;
    TextCopy(temp.text, text);
    temp.size = size;
    temp.color = color;
    temp.wrap = wrap;
    temp.spacing = spacing;
    return temp;
}
GuiObject assignGuiObject(GuiBox box, GuiText textBox, bool hidden, Vector2 offset){
    GuiObject temp;
    temp.textBox = textBox;
    temp.box = box;
    temp.hidden = hidden;
    temp.offset = offset;
    return temp;
}

GuiInteract assignGuiInteract(GuiButton button, GuiText textBox, bool hidden, Vector2 offset){
    GuiInteract temp;
    temp.textBox = textBox;
    temp.button = button;
    temp.hidden = hidden;
    temp.offset = offset;
    return temp;
}

bool guiButtonPressed(GuiButton input, int mouseButton){
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    if(IsMouseButtonDown(mouseButton) && mouseX > input.rect.x && mouseY > input.rect.y && mouseX < input.rect.width + input.rect.x && mouseY < input.rect.height + input.rect.y){
        return true;
    }else{
        return false;
    }
}