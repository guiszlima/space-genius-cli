#include <stdio.h>
#include <stdlib.h>
#include "../../Repository/Repository.h"
#include "MenuController.h"
#include "../User/UserController.h"
#include "../Auth/Auth.h"
#include "../Profile/ProfileController.h"
#include "../../Utils/Input.h"
#include "../Category/CategoryController.h"
#include "../Game/GameController.h"
#include "../Cart/CartController.h"
#include "../Balance/BalanceController.h"
#include "../Review/ReviewController.h"
#include "../../Wishlist/WishlistController.h"
#include "../Store/StoreController.h"
#include "../UserAdmin/UserAdminController.h"
#include <sqlite3.h>

// Callback para obter o campo admin da consulta
static int admin_check_callback(void *data, int argc, char **argv, char **azColName)
{
    int *is_admin = (int *)data;
    if (argc > 0 && argv[0])
    {
        *is_admin = atoi(argv[0]);
    }
    return 0;
}

// Protótipos de funções de placeholder para as novas opções
void show_admin_menu();

static int is_user_admin(int user_id)
{
    int is_admin = 0;
    char *sql = sqlite3_mprintf("SELECT admin FROM usuarios WHERE id = %d LIMIT 1;", user_id);
    if (!sql)
        return 0;

    execute_query(sql, admin_check_callback, &is_admin);
    sqlite3_free(sql);
    return is_admin == 1;
}

void show_main_menu(void)
{
    int option;

    printf("\n===== PLAY STORE CLI =====\n");

    if (!auth_is_logged_in())
    {
        printf("1. Login\n");
        printf("2. Registrar\n");
        printf("0. Sair\n");
    }
    else
    {
        UsuarioLogado *user = auth_get_usuario_logado();
        printf("Bem-vindo, %s! (Saldo: R$%.2f)\n", user->username, user->saldo);
        printf("--------------------------\n");
        printf("1. 📁 Perfil\n");
        printf("2. 🛒 Carrinho\n");
        printf("3. ❤️ Lista de desejos\n");
        printf("4. ⭐ Minhas Avaliações\n");
        printf("5. 🕹 Loja de Jogos\n");
        printf("6. 💰 Saldo / Transações\n");
        // mostra opção admin somente se a query confirmar admin = 1
        if (is_user_admin(user->id))
        {
            printf("7. 🛠️ Menu Admin\n");
        }
        printf("0. 🚪 Logout\n");
    }

    printf("==========================\n");

    option = get_menu_option("Escolha uma opção: ");

    if (!auth_is_logged_in())
    {
        switch (option)
        {
        case 1:
            login_user_flow();
            break;
        case 2:
            register_user_flow();
            break;
        case 0:
            printf("Saindo...\n");
            exit(0);
        default:
            printf("Opção inválida! Tente novamente.\n");
            break;
        }
    }
    else
    {
        switch (option)
        {
        case 1:
            show_profile_menu();
            break;
        case 2:
            show_cart_menu();
            break;
        case 3:
            show_wishlist_menu();
            break;
        case 4:
            show_review_menu();
            break;
        case 5:
            show_store_menu();
            break;
        case 6:
            show_balance_menu();
            break;
        case 7:
        {
            UsuarioLogado *user = auth_get_usuario_logado();
            if (!is_user_admin(user->id))
            {
                printf("Opção Inválida.\n");
                break;
            }
            show_admin_menu();
            break;
        }
        case 0:
            auth_logout();
            break;

        default:
            printf("Opção inválida! Tente novamente.\n");
            break;
        }
    }
}

void show_admin_menu()
{
    int option = -1;
    while (option != 0)
    {
        printf("\n--- MENU ADMINISTRATIVO ---\n");
        printf("1. Gerenciar Categorias\n");
        printf("2. Gerenciar Jogos\n");
        printf("3. Gerenciar Usuários\n");

        printf("0. Voltar\n");
        printf("---------------------------\n");
        option = get_menu_option("Escolha uma opção: ");
        switch (option)
        {
        case 1:
            show_category_menu();
            break;
        case 2:
            show_game_menu();
            break;
        case 3:
            show_user_admin_menu();
            break;

        case 0:
            break;
        default:
            printf("Opção inválida.\n");
        }
    }
}
