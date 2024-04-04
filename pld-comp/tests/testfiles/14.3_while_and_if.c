int main()
{
    int a = 1;
    int b = 1;

    while (a < 5) {
        if (b < 5) {
            b = b + 1;
        } else {
            a = a + 1;
        }
    }

    return 0;
}