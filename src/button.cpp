#include <string.h>
#include "DxLib\DxLib.h"
#include "button.h"

extern int font_UI;

void Button::init(int x, int y, int width, int height, const char *str){
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	drawedStr = str;
	pushed = false;
	font = font_UI;
}

bool Button::isClicked(){
	int mouseX, mouseY;

	GetMousePoint(&mouseX, &mouseY);

	if(x < mouseX && x + width > mouseX && y < mouseY && y + height > mouseY){ // mouse in button
		if(pushed){
			if(!(GetMouseInput() & MOUSE_INPUT_LEFT)){
				pushed = false;
				return true;
			}
		}else{
			if(GetMouseInput() & MOUSE_INPUT_LEFT)pushed = true;
		}
	}else{ // mouse out button
		if(pushed){
			pushed = false;
		}
	}

	return false;
}

void Button::draw(){
	if(pushed)DrawBox(x, y, x + width, y + height, GetColor(100, 100, 100), TRUE);
	else DrawBox(x, y, x + width, y + height, GetColor(150, 150, 150), TRUE);
	DrawBox(x, y, x + width, y + height, GetColor(0, 0, 0), FALSE);
	DrawStringToHandle(x + width / 2 - GetDrawStringWidthToHandle(drawedStr, strlen(drawedStr), font) / 2, y + height / 2 - 10, drawedStr, GetColor(0, 0, 0), font);
}

void Button::reLocate(int x, int y){
	this->x = x;
	this->y = y;
}

void Button::setString(const char *str){
	drawedStr = str;
}