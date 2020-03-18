#include "colorButton.h"

void ColorButton::init(int x, int y, int width, int height, const char *str, int r, int g, int b){
    this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	drawedStr = str;
	pushed = false;
    this->r = r;
    this->g = g;
    this->b = b;    
}

int upzero(int x){
    if(x > 0)return x;
    else return 0;
}

void ColorButton::draw(){
    if(pushed == false)DrawBox(x, y, x + width, y + height, GetColor(r, g, b), TRUE);
	else DrawBox(x, y, x + width, y + height, GetColor(upzero(r - 100), upzero(g - 100), upzero(b - 100)), TRUE);
	DrawBox(x, y, x + width, y + height, GetColor(0, 0, 0), FALSE);
	DrawString(x + width / 2 - GetDrawStringWidthToHandle((const TCHAR *)drawedStr, strlen(drawedStr), font) / 2, y + height / 2 - 12, (const TCHAR *)drawedStr, GetColor(0, 0, 0));
}