#include <stdio.h>
#include <stdlib.h>
#include "../Database/Database.h"
#include "../Migrate/migrate.h"
#include "../Controllers/Menu/MenuController.h"
#include "../Utils/Colors.h"

void cleanup()
{
    db_close();
    printf("%sConexão com o banco de dados fechada.%s\n", C_INFO, C_RESET);
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
