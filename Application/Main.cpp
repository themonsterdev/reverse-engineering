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
