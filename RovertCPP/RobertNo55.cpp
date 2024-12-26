// File3b.cpp
#include <stdio.h>
#include <string.h>

int main()
{
    FILE *pFile;
    char  buffer[512];
    int   nLength;

    printf("何か文字列を入力して下さい > ");
    fgets(buffer, sizeof(buffer), stdin);
    printf("%d",buffer[0]);

    pFile = fopen("fwrite.txt", "wb");

    nLength = strlen(buffer);
    fwrite(&nLength, 4, 1, pFile);
    fwrite(buffer, 1, nLength + 1, pFile);

    fclose(pFile);

    return 0;
}




