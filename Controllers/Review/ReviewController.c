#include "ReviewController.h"
#include "../../Utils/Input.h"
#include "../../Repository/Repository.h"
#include "../Auth/Auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>


void rate_game();
void list_reviews_by_game();
void edit_my_review();
void delete_my_review();

void show_review_menu()
{
    int option = -1;

    while (option != 0)
    {
        printf("\n--- MENU DE AVALIAÇÕES ---\n");
        printf("1. Avaliar um Jogo\n");
        printf("2. Listar Avaliações de um Jogo\n");
        printf("3. Editar uma de Minhas Avaliações\n");
        printf("4. Remover uma de Minhas Avaliações\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("-----------------------------\n");

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            rate_game();
            break;
        case 2:
            list_reviews_by_game();
            break;
        case 3:
            edit_my_review();
            break;
        case 4:
            delete_my_review();
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

void rate_game()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você precisa estar logado para avaliar um jogo.\n");
        return;
    }

    
    
    extern void list_games();
    list_games();

    char game_id_buffer[10], rating_buffer[10], comment[501];
    int game_id, rating;

    get_input("Digite o ID do jogo que deseja avaliar: ", game_id_buffer, sizeof(game_id_buffer));
    if (sscanf(game_id_buffer, "%d", &game_id) != 1)
    {
        printf("ID de jogo inválido.\n");
        return;
    }

    get_input("Digite sua nota (1 a 5): ", rating_buffer, sizeof(rating_buffer));
    if (sscanf(rating_buffer, "%d", &rating) != 1 || rating < 1 || rating > 5)
    {
        printf("Nota inválida. Deve ser um número entre 1 e 5.\n");
        return;
    }

    get_input("Deixe um comentário (opcional): ", comment, sizeof(comment));

    
    
    char *sql = sqlite3_mprintf(
        "INSERT INTO avaliacoes (usuario_id, jogo_id, nota, comentario) VALUES (%d, %d, %d, '%q');",
        user->id, game_id, rating, comment);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("Avaliação registrada com sucesso!\n");
    }
    else
    {
        printf("Erro ao registrar avaliação. Verifique se o ID do jogo é válido.\n");
    }

    sqlite3_free(sql);
}


static int list_reviews_callback(void *data, int argc, char **argv, char **azColName)
{
    
    printf("%-5s | %-20s | %-5s | %-40s | %-20s\n",
           argv[0], argv[1], argv[2], argv[3], argv[4]);
    return 0;
}

void list_reviews_by_game()
{
    extern void list_games();
    list_games();

    char game_id_buffer[10];
    int game_id;

    get_input("Digite o ID do jogo para ver as avaliações: ", game_id_buffer, sizeof(game_id_buffer));
    if (sscanf(game_id_buffer, "%d", &game_id) != 1)
    {
        printf("ID de jogo inválido.\n");
        return;
    }

    printf("\n--- AVALIAÇÕES PARA O JOGO ID %d ---\n", game_id);

    char *sql = sqlite3_mprintf(
        "SELECT a.id, u.username, a.nota, a.comentario, a.criado_em "
        "FROM avaliacoes a JOIN usuarios u ON a.usuario_id = u.id "
        "WHERE a.jogo_id = %d ORDER BY a.criado_em DESC;",
        game_id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    printf("\n%-5s | %-20s | %-5s | %-40s | %-20s\n", "ID", "Usuário", "Nota", "Comentário", "Data");
    printf("------------------------------------------------------------------------------------------------------\n");
    execute_query(sql, list_reviews_callback, NULL);
    printf("------------------------------------------------------------------------------------------------------\n");

    sqlite3_free(sql);
}


static int list_my_reviews_callback(void *data, int argc, char **argv, char **azColName)
{
    
    printf("%-5s | %-30s | %-5s | %-40s | %-20s\n",
           argv[0], argv[1], argv[2], argv[3], argv[4]);
    return 0;
}

void edit_my_review()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você precisa estar logado para editar suas avaliações.\n");
        return;
    }

    printf("\n--- MINHAS AVALIAÇÕES ---\n");
    char *sql_list = sqlite3_mprintf(
        "SELECT a.id, j.nome, a.nota, a.comentario, a.criado_em "
        "FROM avaliacoes a JOIN jogos j ON a.jogo_id = j.id "
        "WHERE a.usuario_id = %d ORDER BY a.criado_em DESC;",
        user->id);

    if (!sql_list)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    printf("\n%-5s | %-30s | %-5s | %-40s | %-20s\n", "ID", "Jogo", "Nota", "Comentário", "Data");
    printf("------------------------------------------------------------------------------------------------------\n");
    execute_query(sql_list, list_my_reviews_callback, NULL);
    printf("------------------------------------------------------------------------------------------------------\n");
    sqlite3_free(sql_list);

    char review_id_buffer[10], rating_buffer[10], comment[501];
    int review_id, rating;

    get_input("Digite o ID da avaliação que deseja editar: ", review_id_buffer, sizeof(review_id_buffer));
    if (sscanf(review_id_buffer, "%d", &review_id) != 1)
    {
        printf("ID de avaliação inválido.\n");
        return;
    }

    get_input("Digite a nova nota (1 a 5): ", rating_buffer, sizeof(rating_buffer));
    if (sscanf(rating_buffer, "%d", &rating) != 1 || rating < 1 || rating > 5)
    {
        printf("Nota inválida. Deve ser um número entre 1 e 5.\n");
        return;
    }

    get_input("Deixe o novo comentário (opcional): ", comment, sizeof(comment));

    char *sql_update = sqlite3_mprintf(
        "UPDATE avaliacoes SET nota = %d, comentario = '%q' WHERE id = %d AND usuario_id = %d;",
        rating, comment, review_id, user->id);

    if (!sql_update)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql_update) > 0)
    {
        printf("Avaliação atualizada com sucesso!\n");
    }
    else
    {
        printf("Erro ao atualizar avaliação. Verifique se o ID da avaliação é seu.\n");
    }

    sqlite3_free(sql_update);
}

void delete_my_review()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você precisa estar logado para remover suas avaliações.\n");
        return;
    }

    printf("\n--- REMOVER MINHA AVALIAÇÃO ---\n");
    char *sql_list = sqlite3_mprintf(
        "SELECT a.id, j.nome, a.nota, a.comentario, a.criado_em "
        "FROM avaliacoes a JOIN jogos j ON a.jogo_id = j.id "
        "WHERE a.usuario_id = %d ORDER BY a.criado_em DESC;",
        user->id);

    if (!sql_list)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    printf("\n%-5s | %-30s | %-5s | %-40s | %-20s\n", "ID", "Jogo", "Nota", "Comentário", "Data");
    printf("------------------------------------------------------------------------------------------------------\n");
    execute_query(sql_list, list_my_reviews_callback, NULL);
    printf("------------------------------------------------------------------------------------------------------\n");
    sqlite3_free(sql_list);

    char review_id_buffer[10];
    int review_id;

    get_input("Digite o ID da avaliação que deseja remover: ", review_id_buffer, sizeof(review_id_buffer));
    if (sscanf(review_id_buffer, "%d", &review_id) != 1)
    {
        printf("ID de avaliação inválido.\n");
        return;
    }

    char *sql_delete = sqlite3_mprintf(
        "DELETE FROM avaliacoes WHERE id = %d AND usuario_id = %d;",
        review_id, user->id);

    if (!sql_delete)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql_delete) > 0)
    {
        printf("Avaliação removida com sucesso!\n");
    }
    else
    {
        printf("Erro ao remover avaliação. Verifique se o ID da avaliação é seu.\n");
    }

    sqlite3_free(sql_delete);
}
