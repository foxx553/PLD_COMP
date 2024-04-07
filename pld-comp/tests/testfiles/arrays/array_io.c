#include <stdio.h>

void read(int* array, int n)
{
    int i = 0;
    while(i < n)
    {
        array[i] = getchar();
        i = i + 1;
    }
}

void print(int* array, int n)
{
    int i = 0;
    while(i < n)
    {
        putchar(array[i]);
        i = i + 1;
    }
}

int main()
{
    int instruction[100];
    instruction[0] = 'e';
    instruction[1] = 'c';
    instruction[2] = 'r';
    instruction[3] = 'i';
    instruction[4] = 'v';
    instruction[5] = 'e';
    instruction[6] = 'z';
    instruction[7] = '!';
    instruction[8] = '\n';
    print(instruction, 9);

    int text[100];
    read(text, 5);
    print(text, 5);
    return 0;
}