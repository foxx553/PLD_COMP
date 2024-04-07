int max(int a, int b)
{
    if(a > b)
    {
        return a;
    }

    return b;
}

int knapsack(int* weights, int* values, int n, int capacity)
{
    int dp[51];
    int i = 0;
    while(i <= 50)
    {
        dp[i] = 0;
        i = i + 1;
    }

    int j = 0;
    while(j < n)
    {
        int k = 50;
        while(k >= weights[j])
        {
            dp[k] = max(dp[k], dp[k - weights[j]] + values[j]);
            k = k - 1;
        }

        j = j + 1;
    }

    return dp[capacity];
}

int main()
{
    int weights[10];
    weights[0] = 10;
    weights[1] = 20;
    weights[2] = 30;

    int values[10];
    values[0] = 60;
    values[1] = 100;
    values[2] = 120;

    return knapsack(weights, values, 3, 50);
}