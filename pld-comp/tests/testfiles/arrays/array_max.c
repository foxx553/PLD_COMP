int main()
{
    int a[5];
    a[0] = 15;
    a[1] = 20;
    a[2] = 4;
    a[3] = 33;
    a[4] = 9;
    int i = 1, max = a[0];

    while(i < 5)
    {
        if(a[i] > max)
        {
            max = a[i];
        }
        i = i + 1;
    }

    return max;
}