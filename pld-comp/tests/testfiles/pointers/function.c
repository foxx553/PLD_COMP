int f(int* a)
{
    return *a;
}

int main()
{
    int a = 5;
    int* b = &a;
    return f(b);
}
