#include "Input.h"
#include <stdio.h>
#include <string.h>

void get_input(const char *prompt, char *buffer, int size)
{
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = 0;
    }
}

int get_menu_option(const char *prompt)
{
    char buffer[10];
    int option;

    printf("%s", prompt);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        if (sscanf(buffer, "%d", &option) == 1)
        {
            return option;
        }
    }
    return -1; // Invalid option
}
