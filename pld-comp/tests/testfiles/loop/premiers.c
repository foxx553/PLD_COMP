int main()
{
    int nombre = 2;
    int somme = 0;

    while(nombre <= 20)
    {
        int estPremier = 1;

        int diviseur = 2;
        while(diviseur <= (nombre / 2))
        {
            if((nombre % diviseur) == 0)
            {
                estPremier = 0;
                break;
            }
            diviseur = diviseur + 1;
        }

        if(estPremier)
        {
            somme = somme + nombre;
        }

        nombre = nombre + 1;
    }

    return somme;
}