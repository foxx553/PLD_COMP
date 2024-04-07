int min(int a, int b) {
    if (a > b) {
        return b;
    } 

    return a;
}

int main() {
    int x = 1;
    int y = 2;
    int z = min(x, y);
    return z;
}