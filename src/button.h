#ifndef BUTTON_H
#define BUTTON_H

#include <string.h>
#include "DxLib\DxLib.h"

class Button{
	public:
		Button(){};
		virtual void init(int x, int y, int width, int height, const char *str);
		bool isClicked();
		virtual void draw();
		void reLocate(int x, int y);
		void setString(const char *str);
	protected:
		int x, y;
		int height, width;
		bool pushed;
		const char *drawedStr;
		int font;
};

#endif