#include <stdio.h>

int main()
{
    int text['\n']; // \n vaut 10 en ASCII
    text[0] = 'h';
    text[1] = 'e';
    text[2] = 'x';
    text[3] = 'a';
    text[4] = 'g';
    text[5] = 'a';
    text[6] = 'n';
    text[7] = 'g';
    text[8] = '!';
    text[9] = '\0';

    int i = 0;
    while(i < 10)
    {
        putchar(text[i]);
        i = i + 1;
    }

    return 0;
}