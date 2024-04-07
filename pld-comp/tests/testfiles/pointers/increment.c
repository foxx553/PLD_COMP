int increment(int* a)
{
    *a = *a + 1;
}

int main()
{
    int a = 5;
    increment(&a);
    increment(&a);
    increment(&a);
    increment(&a);
    increment(&a);
    return a;
}
