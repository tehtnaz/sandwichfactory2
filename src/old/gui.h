//gui.h version 1.1
//updated formatting 1.0.1
// turned into seperate c file 1.1

#ifndef GUI_H
    #define GUI_H

    #include "raylib.h"
    #include "vector2pp.h"
    #include "recpp.h"

typedef struct GuiBox{
    int borderWidth;
    Rectangle rect;
    //float rotation;
    Color color;
    //Color borderColor;
}GuiBox;

typedef struct GuiImg{
    Texture2D img;
    Vector2 position;
}GuiImg;

typedef struct GuiText{
    Vector2 position;
    int size;
    char text[50];
    Color color;

    bool wrap;
    float spacing;
}GuiText;

typedef struct GuiObject{
    Vector2 offset;
    GuiBox box;
    GuiText textBox;
    bool hidden;
}GuiObject;

typedef struct GuiButton{
    Rectangle rect;
    //GuiText buttonText;
    int borderWidth;
    Color color;
    bool isPressed;
}GuiButton;

typedef struct GuiInteract{
    Vector2 offset;
    GuiButton button;
    GuiText textBox;
    bool hidden;
}GuiInteract;

void renderGuiObject(GuiObject input);
void renderGuiInteract(GuiInteract input);
void renderGuiText(GuiText input);
void renderGuiButton(GuiButton input);

GuiBox assignGuiBox(int borderWidth, Rectangle rect, Color color);
GuiButton assignGuiButton(int borderWidth, Rectangle rect, Color color);
GuiText assignGuiText(Vector2 position, char text[50], int size, Color color, bool wrap, float spacing);
GuiObject assignGuiObject(GuiBox box, GuiText textBox, bool hidden, Vector2 offset);
GuiInteract assignGuiInteract(GuiButton button, GuiText textBox, bool hidden, Vector2 offset);

bool guiButtonPressed(GuiButton input, int mouseButton);


//Use is discouraged

/*typedef struct GuiMulti{
    GuiBox box;
    GuiText* textBox; //pointer so that it can be an array
    bool hidden;
    bool hasText;
}GuiMulti;*/

#endif