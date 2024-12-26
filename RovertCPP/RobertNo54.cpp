// File3.cpp
#include <stdio.h>
#include <string.h>

int main()
{
    FILE* pFile;
    char  buffer[512];
    int   i;

    printf("何か文字列を入力して下さい > ");
    fgets(buffer, sizeof(buffer), stdin);

    pFile = fopen("fprintf.txt", "w");
    fprintf(pFile, "%ld\n", strlen(buffer));
    for(i = 0u; buffer[i]; i++)
        fprintf(pFile, "%02X ", (unsigned char)buffer[i]);
    fclose(pFile);

    return 0;
}
