# Application

## Sommaire

- Introduction
- Function main
- Fonction sum
- Code complet

## Introduction

L'application est un exemple de programme simple qui affichera 5 + 5 tout les seconde.

## Fonction main

La fonction `main` est le point d'entrée de notre application, c'est ici que nous alons faire une boucle qui
affichera le résultat de `5 + 5`.

```c++
// Entry point of the application.
int main()
{
    while (true)
    {
        // Imprime le résultat de 5 + 5 une fois par seconde.
        printf("sum 5 + 5 = %d\n", sum(5, 5));

        // Attend 1 seconde avant de continuer.
        Sleep(1000);
    }

    // Retourne un status de sortie avec success.
    return EXIT_SUCCESS;
}
```

## Fonction sum

La fonction `sum` permet de calculer la somme de deux valeurs donnée.

```c++
// Fonction qui sera accrochée par notre dll.
int sum(int x, int y)
{
    return x + y;
}
```

## Code complet

```c++
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Function that will be hooked by our dll.
int sum(int x, int y)
{
    return x + y;
}

// Cross-platform sleep function.
// https://stackoverflow.com/questions/4184468/sleep-for-milliseconds
void Sleep(int milliseconds)
{
    clock_t time_end;
    time_end = clock() + milliseconds * CLOCKS_PER_SEC / 1000;
    while (clock() < time_end);
}

// Entry point of the application.
int main()
{
    while (true)
    {
        // Prints the result of 5 + 5 once per second.
        printf("sum 5 + 5 = %d\n", sum(5, 5));

        // Wait 1 second before continuing
        Sleep(1000);
    }

    return EXIT_SUCCESS;
}
```
