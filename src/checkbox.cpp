#include "checkbox.h"

extern int font_emoji;

void CheckBox::setStatus(bool nextStat){
    status = nextStat;
}

void CheckBox::reverse(){
    status = status ? false : true;
}

void CheckBox::draw(){
    unsigned int color;
    int size = (width > height) ? width : height;
    int x1, y1;
    int x2, y2;
    int x3, y3;
    ColorButton::draw();
    color = (r + g + b / 2 < 150) ? GetColor(255, 255, 255) : GetColor(0, 0, 0);
    if(status == true){
        x1 = x + width / 2 - (int)(12.0 * ((float)size / 32.0));
        y1 = y + height / 2 - (int)(3.0 * ((float)size / 32.0));
        x2 = x + width / 2 - (int)(3.0 * ((float)size / 32.0));
        y2 = y + height / 2 + (int)(8.0 * ((float)size / 32.0));
        x3 = x + width / 2 + (int)(12.0 * ((float)size / 32.0));
        y3 = y + height / 2 - (int)(7.0 * ((float)size / 32.0));
        DrawLineAA(x1, y1, x2, y2, color);
        DrawLineAA(x2, y2, x3, y3, color);
    }
}