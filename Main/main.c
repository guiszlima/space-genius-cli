#include <stdio.h>
#include <stdlib.h>
#include "../Database/Database.h"
#include "../Migrate/migrate.h"
#include "../Controllers/Menu/MenuController.h"


void cleanup()
{
    db_close();
    printf("Conexão com o banco de dados fechada.\n");
}

int main()
{
    
    atexit(cleanup);

    
    db_connect();

    
    run_migrations();

    
    while (1)
    {
        show_main_menu();
    }

    return 0; 
}
