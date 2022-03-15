# Application

## Sommaire

- Introduction

## Introduction

L'application est un exemple de programme simple qui affichera 5 + 5 tout les seconde.

```c++
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Fonction qui sera accrochée par notre dll.
int sum(int x, int y)
{
    return x + y;
}

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
