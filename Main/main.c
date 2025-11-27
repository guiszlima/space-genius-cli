#include <stdio.h>
#include <stdlib.h>
#include "../Database/Database.h"
#include "../Migrate/migrate.h"
#include "../Controllers/Menu/MenuController.h"

// Função para limpar recursos antes de sair
void cleanup()
{
    db_close();
    printf("Conexão com o banco de dados fechada.\n");
}

int main()
{
    // Registrar a função de limpeza para ser chamada na saída
    atexit(cleanup);

    // 1. Inicializa a conexão com o banco de dados
    db_connect();

    // 2. Executa as migrações
    run_migrations();

    // 3. Loop principal da aplicação
    while (1)
    {
        show_main_menu();
    }

    return 0; // Nunca é atingido, mas é uma boa prática
}
