int main()
{
    int a = 1;
    int b = 1;

    while (a < 12) {
        b = b + 1;
        a = a * b;
    }

    return a;
}