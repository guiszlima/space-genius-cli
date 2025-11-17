#include <stdio.h>
#include "../Migrate/migrate.h"
#include "../Controllers/MenuController.h"

int main()
{
    // 1. Inicializa o banco de dados (carrega ou cria)
    migrate();

    // 2. Loop principal da aplicação
    while (1)
    {
        show_main_menu();
    }

    return 0; // Nunca atingido devido ao loop infinito
}
