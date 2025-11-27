#include "LogController.h"
#include "../Utils/Input.h"
#include "../Repository/Repository.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// Protótipos
void list_admin_logs();

void register_admin_log(const char *admin, const char *action, const char *target)
{
    char *sql = sqlite3_mprintf(
        "INSERT INTO logs_admin (admin, acao, alvo) VALUES ('%q', '%q', '%q');",
        admin, action, target);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL de log.\n");
        return;
    }

    execute_non_query(sql); // Não precisamos verificar o retorno para logs
    sqlite3_free(sql);
}

// Callback para listar logs
static int list_logs_callback(void *data, int argc, char **argv, char **azColName)
{
    // argv[0]: id, argv[1]: admin, argv[2]: acao, argv[3]: alvo, argv[4]: criado_em
    printf("%-5s | %-15s | %-30s | %-20s | %-20s\n",
           argv[0], argv[1], argv[2], argv[3], argv[4]);
    return 0;
}

void show_log_menu()
{
    int option = -1;

    while (option != 0)
    {
        printf("\n--- MENU DE LOGS ADMINISTRATIVOS ---\n");
        printf("1. Listar Logs\n");
        printf("0. Voltar ao Menu Admin\n");
        printf("------------------------------------\n");

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            list_admin_logs();
            break;
        case 0:
            printf("Voltando ao menu admin...\n");
            break;
        default:
            printf("Opção inválida!\n");
            break;
        }
    }
}

void list_admin_logs()
{
    printf("\n--- LISTA DE LOGS ADMINISTRATIVOS ---\n");

    char *sql = "SELECT id, admin, acao, alvo, criado_em FROM logs_admin ORDER BY criado_em DESC;";

    printf("\n%-5s | %-15s | %-30s | %-20s | %-20s\n", "ID", "Admin", "Ação", "Alvo", "Data");
    printf("--------------------------------------------------------------------------------------------------\n");
    execute_query(sql, list_logs_callback, NULL);
    printf("--------------------------------------------------------------------------------------------------\n");
}
