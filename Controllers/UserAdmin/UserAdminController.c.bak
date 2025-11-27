#include "UserAdminController.h"
#include "../../Utils/Input.h"
#include "../../Repository/Repository.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// Protótipos
void list_users();
void edit_user_balance();
void delete_user();
void edit_user_admin();

// Callback para listar usuários
static int list_users_callback(void *data, int argc, char **argv, char **azColName)
{
    // argv[0]: id, argv[1]: username, argv[2]: email, argv[3]: saldo, argv[4]: admin, argv[5]: criado_em
    const char *id = (argc > 0 && argv[0]) ? argv[0] : "NULL";
    const char *username = (argc > 1 && argv[1]) ? argv[1] : "";
    const char *email = (argc > 2 && argv[2]) ? argv[2] : "";
    const char *saldo = (argc > 3 && argv[3]) ? argv[3] : "0.00";
    const char *admin = (argc > 4 && argv[4]) ? argv[4] : "0";
    const char *criado_em = (argc > 5 && argv[5]) ? argv[5] : "";

    printf("%-5s | %-20s | %-30s | R$ %-10s | %-5s | %-20s\n",
           id, username, email, saldo, admin, criado_em);
    return 0;
}

void show_user_admin_menu()
{
    int option = -1;

    while (option != 0)
    {
        printf("\n--- GERENCIAR USUÁRIOS ---\n");
        printf("1. Listar Usuários\n");
        printf("2. Editar Saldo de Usuário\n");
        printf("3. Deletar Usuário\n");
        printf("4. Editar Status de Administrador do Usuário\n");
        printf("0. Voltar ao Menu Admin\n");
        printf("--------------------------\n");

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            list_users();
            break;
        case 2:
            edit_user_balance();
            break;
        case 3:
            delete_user();
            break;
        case 4:
            edit_user_admin();
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

void list_users()
{
    printf("\n--- LISTA DE USUÁRIOS ---\n");

    char *sql = "SELECT id, username, email, saldo, admin, criado_em FROM usuarios ORDER BY username;";

    printf("\n%-5s | %-20s | %-30s | %-11s | %-6s | %-20s\n", "ID", "Username", "Email", "Saldo", "Admin", "Criado Em");
    printf("--------------------------------------------------------------------------------------------------\n");
    execute_query(sql, list_users_callback, NULL);
    printf("--------------------------------------------------------------------------------------------------\n");
}

void edit_user_balance()
{
    int user_id;
    double new_balance;
    char id_buffer[10], balance_buffer[20];

    printf("\n--- EDITAR SALDO DE USUÁRIO ---\n");
    list_users();

    get_input("Digite o ID do usuário para editar o saldo: ", id_buffer, sizeof(id_buffer));
    if (sscanf(id_buffer, "%d", &user_id) != 1)
    {
        printf("ID de usuário inválido.\n");
        return;
    }

    get_input("Digite o novo saldo: R$", balance_buffer, sizeof(balance_buffer));
    if (sscanf(balance_buffer, "%lf", &new_balance) != 1 || new_balance < 0)
    {
        printf("Saldo inválido. Deve ser um número positivo.\n");
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE usuarios SET saldo = %.2f WHERE id = %d;", new_balance, user_id);
    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("Saldo do usuário ID %d atualizado para R$%.2f com sucesso!\n", user_id, new_balance);
    }
    else
    {
        printf("Erro ao atualizar saldo do usuário. Verifique se o ID do usuário é válido.\n");
    }

    sqlite3_free(sql);
}

void delete_user()
{
    int user_id;
    char id_buffer[10];

    printf("\n--- DELETAR USUÁRIO ---\n");
    list_users();

    get_input("Digite o ID do usuário que deseja deletar: ", id_buffer, sizeof(id_buffer));
    if (sscanf(id_buffer, "%d", &user_id) != 1)
    {
        printf("ID de usuário inválido.\n");
        return;
    }

    char confirm[10];
    printf("Tem certeza que deseja deletar o usuário com ID %d? (s/n): ", user_id);
    get_input("", confirm, sizeof(confirm));

    if (strcmp(confirm, "s") != 0 && strcmp(confirm, "S") != 0)
    {
        printf("Deleção cancelada.\n");
        return;
    }

    char *sql = sqlite3_mprintf("DELETE FROM usuarios WHERE id = %d;", user_id);
    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("Usuário ID %d deletado com sucesso!\n", user_id);
    }
    else
    {
        printf("Erro ao deletar usuário. Verifique se o ID existe e se o usuário não possui dados relacionados (carrinho, transações, avaliações, etc.).\n");
    }

    sqlite3_free(sql);
}
void edit_user_admin()
{
    int user_id;
    int is_admin;
    char id_buffer[10], admin_buffer[10];

    printf("\n--- EDITAR STATUS DE ADMINISTRADOR DO USUÁRIO ---\n");
    list_users();

    get_input("Digite o ID do usuário para editar o status de administrador: ", id_buffer, sizeof(id_buffer));
    if (sscanf(id_buffer, "%d", &user_id) != 1)
    {
        printf("ID de usuário inválido.\n");
        return;
    }

    get_input("O usuário deve ser administrador? (1 para Sim, 0 para Não): ", admin_buffer, sizeof(admin_buffer));
    if (sscanf(admin_buffer, "%d", &is_admin) != 1 || (is_admin != 0 && is_admin != 1))
    {
        printf("Entrada inválida. Digite 1 para Sim ou 0 para Não.\n");
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE usuarios SET admin = %d WHERE id = %d;", is_admin, user_id);
    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("Status de administrador do usuário ID %d atualizado com sucesso!\n", user_id);
    }
    else
    {
        printf("Erro ao atualizar status de administrador do usuário. Verifique se o ID do usuário é válido.\n");
    }

    sqlite3_free(sql);
}