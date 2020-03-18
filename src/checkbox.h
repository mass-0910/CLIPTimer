#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "colorButton.h"

class CheckBox : public ColorButton{
    public:
        void setStatus(bool nextStat);
        void reverse();
        bool getStatus(){return status;};
        void draw();
    private:
        bool status = false;
};

#endif