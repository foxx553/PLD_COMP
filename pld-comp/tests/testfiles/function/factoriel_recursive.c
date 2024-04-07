int factorielle(int n) {
    if (n == 0){
        return 1;
    }else{
        return n * factorielle(n - 1);
    }
}

int main() {

    int resultat = factorielle(5);

    return resultat;
}