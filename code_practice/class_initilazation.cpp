#include <stdio.h>

class POS
{
public:
    int x;
    int y;

    POS(void);
};

// �R���X�g���N�^��`
POS::POS(void)
{
    x = 0;
    y = 0;
}

int main()
{
    POS pos;

    return 0;
}