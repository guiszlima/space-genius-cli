#include "WishlistController.h"
#include "../Utils/Input.h"
#include "../Repository/Repository.h"
#include "../Controllers/Auth/Auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// Protótipos
void add_to_wishlist();
void list_wishlist_items();
void remove_from_wishlist();

// Callback para listar itens da lista de desejos
static int list_wishlist_callback(void *data, int argc, char **argv, char **azColName)
{
    // argv[0]: id, argv[1]: jogo_nome, argv[2]: adicionado_em
    printf("%-5s | %-30s | %-20s\n",
           argv[0], argv[1], argv[2]);
    return 0;
}

void show_wishlist_menu()
{
    int option = -1;

    while (option != 0)
    {
        printf("\n--- SUA LISTA DE DESEJOS ---\n");
        list_wishlist_items(); // Sempre lista os itens ao entrar no menu
        printf("------------------------------\n");
        printf("1. Adicionar Jogo à Lista de Desejos\n");
        printf("2. Remover Jogo da Lista de Desejos\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("------------------------------\n");

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            add_to_wishlist();
            break;
        case 2:
            remove_from_wishlist();
            break;
        case 0:
            printf("Voltando ao menu principal...\n");
            break;
        default:
            printf("Opção inválida!\n");
            break;
        }
    }
}

void add_to_wishlist()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você precisa estar logado para adicionar jogos à lista de desejos.\n");
        return;
    }

    extern void list_games();
    list_games();

    char game_id_buffer[10];
    int game_id;

    get_input("Digite o ID do jogo para adicionar à lista de desejos: ", game_id_buffer, sizeof(game_id_buffer));
    if (sscanf(game_id_buffer, "%d", &game_id) != 1)
    {
        printf("ID de jogo inválido.\n");
        return;
    }

    char *sql = sqlite3_mprintf(
        "INSERT INTO lista_desejos (usuario_id, jogo_id) VALUES (%d, %d);",
        user->id, game_id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("Jogo adicionado à lista de desejos com sucesso!\n");
    }
    else
    {
        printf("Erro ao adicionar jogo à lista de desejos. Verifique se o ID do jogo é válido ou se já está na sua lista.\n");
    }

    sqlite3_free(sql);
}

void list_wishlist_items()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você não está logado.\n");
        return;
    }

    char *sql = sqlite3_mprintf(
        "SELECT ld.id, j.nome, ld.adicionado_em "
        "FROM lista_desejos ld JOIN jogos j ON ld.jogo_id = j.id "
        "WHERE ld.usuario_id = %d ORDER BY ld.adicionado_em DESC;",
        user->id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    printf("\n%-5s | %-30s | %-20s\n", "ID", "Jogo", "Adicionado Em");
    printf("------------------------------------------------------------------\n");
    execute_query(sql, list_wishlist_callback, NULL);
    printf("------------------------------------------------------------------\n");

    sqlite3_free(sql);
}

void remove_from_wishlist()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você precisa estar logado para remover jogos da lista de desejos.\n");
        return;
    }

    char wishlist_item_id_buffer[10];
    int wishlist_item_id;

    list_wishlist_items(); // Mostra os itens para o usuário escolher qual remover

    get_input("Digite o ID do item da lista de desejos para remover: ", wishlist_item_id_buffer, sizeof(wishlist_item_id_buffer));
    if (sscanf(wishlist_item_id_buffer, "%d", &wishlist_item_id) != 1)
    {
        printf("ID de item inválido.\n");
        return;
    }

    char *sql = sqlite3_mprintf(
        "DELETE FROM lista_desejos WHERE id = %d AND usuario_id = %d;",
        wishlist_item_id, user->id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("Jogo removido da lista de desejos com sucesso!\n");
    }
    else
    {
        printf("Erro ao remover jogo da lista de desejos. Verifique se o ID do item é válido e pertence à sua lista.\n");
    }

    sqlite3_free(sql);
}
