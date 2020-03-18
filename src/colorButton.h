#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include "button.h"

class ColorButton : public Button{
    public:
        void init(int x, int y, int width, int height, const char *str, int r, int g, int b);
    protected:
        virtual void draw();
        int r, g, b;
};

#endif