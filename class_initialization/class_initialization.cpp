#include <stdio.h>
#include "class_initialization.h"
// �R���X�g���N�^����
POS::POS(int x_init, int y_init)
{
    this ->x = x_init;
    this ->y = y_init;
    printf("x is %d, y is %d", this ->x, this->y);


}