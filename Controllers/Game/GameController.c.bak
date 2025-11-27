#include "GameController.h"
#include "../../Utils/Input.h"
#include "../../Repository/Repository.h"
#include "../Category/CategoryController.h" // Para poder listar as categorias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// Protótipos
void create_game();
void list_games();
void edit_game();
void delete_game();
void manage_related_games();

void show_game_menu()
{
    int option = -1;

    while (option != 0)
    {
        printf("\n--- MENU DE GERENCIAMENTO DE JOGOS ---\n");
        printf("1. Adicionar Jogo\n");
        printf("2. Listar Jogos\n");
        printf("3. Editar Jogo\n");
        printf("4. Deletar Jogo\n");
        printf("5. Gerenciar Jogos Relacionados\n");
        printf("0. Voltar ao Menu Admin\n");
        printf("--------------------------------------\n");

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            create_game();
            break;
        case 2:
            list_games();
            break;
        case 3:
            edit_game();
            break;
        case 4:
            delete_game();
            break;
        case 5:
            manage_related_games();
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

void create_game()
{
    char name[151], description[501], price_str[20];
    int category_id, available;
    double price;

    printf("\n--- ADICIONAR NOVO JOGO ---\n");

    // Listar categorias para ajudar o usuário
    // A função list_categories de CategoryController não é ideal aqui,
    // pois ela é feita para um menu. O ideal seria ter uma função que só lista.
    // Por simplicidade, vamos chamar a função existente.
    printf("Categorias disponíveis:\n");
    list_categories(); // Em um projeto real, isso seria refatorado.

    char cat_id_buffer[10];
    get_input("Digite o ID da categoria: ", cat_id_buffer, sizeof(cat_id_buffer));
    if (sscanf(cat_id_buffer, "%d", &category_id) != 1)
    {
        printf("ID de categoria inválido.\n");
        return;
    }

    get_input("Nome do jogo: ", name, sizeof(name));
    get_input("Descrição: ", description, sizeof(description));
    get_input("Preço (ex: 59.99): ", price_str, sizeof(price_str));
    if (sscanf(price_str, "%lf", &price) != 1)
    {
        printf("Preço inválido.\n");
        return;
    }

    printf("Disponível (1 para Sim, 0 para Não): ");
    char available_buffer[10];
    get_input("", available_buffer, sizeof(available_buffer));
    if (sscanf(available_buffer, "%d", &available) != 1 || (available != 0 && available != 1))
    {
        printf("Valor de disponibilidade inválido.\n");
        return;
    }

    char *sql = sqlite3_mprintf(
        "INSERT INTO jogos (categoria_id, nome, descricao, preco, disponivel) VALUES (%d, '%q', '%q', %f, %d);",
        category_id, name, description, price, available);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) == 0)
    {

        printf("Erro ao adicionar jogo. Verifique se o ID da categoria é válido.\n");
    }
    else
    {
        printf("Jogo '%s' adicionado com sucesso!\n", name);
    }

    sqlite3_free(sql);
}

// Callback para a listagem de jogos
static int list_games_callback(void *data, int argc, char **argv, char **azColName)
{
    // argv[0] = jogo.id, argv[1] = jogo.nome, argv[2] = categoria.nome, argv[3] = preco, argv[4] = disponivel
    printf("%-5s | %-30s | %-20s | R$ %-10s | %-12s\n",
           argv[0], argv[1], argv[2], argv[3], atoi(argv[4]) ? "Sim" : "Não");
    return 0;
}

void list_games()
{
    char filter_choice[10];
    int category_id_filter = -1;

    printf("\n--- LISTA DE JOGOS ---\n");
    get_input("Deseja filtrar por categoria? (s/n): ", filter_choice, sizeof(filter_choice));

    if (strcmp(filter_choice, "s") == 0 || strcmp(filter_choice, "S") == 0)
    {
        printf("Categorias disponíveis:\n");
        list_categories();
        char cat_id_buffer[10];
        get_input("Digite o ID da categoria para filtrar: ", cat_id_buffer, sizeof(cat_id_buffer));
        if (sscanf(cat_id_buffer, "%d", &category_id_filter) != 1)
        {
            printf("ID de categoria inválido. Listando todos os jogos.\n");
            category_id_filter = -1;
        }
    }

    char *sql;
    const char *base_sql = "SELECT j.id, j.nome, c.nome AS categoria, j.preco, j.disponivel "
                           "FROM jogos j JOIN categorias c ON j.categoria_id = c.id";

    if (category_id_filter != -1)
    {
        sql = sqlite3_mprintf("%s WHERE j.categoria_id = %d ORDER BY j.nome;", base_sql, category_id_filter);
    }
    else
    {
        sql = sqlite3_mprintf("%s ORDER BY j.nome;", base_sql);
    }

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    printf("\n%-5s | %-30s | %-20s | %-11s | %-12s\n", "ID", "Nome do Jogo", "Categoria", "Preço", "Disponível");
    printf("--------------------------------------------------------------------------------------------------\n");
    execute_query(sql, list_games_callback, NULL);
    printf("--------------------------------------------------------------------------------------------------\n");

    sqlite3_free(sql);
}

// Struct e callback para buscar os dados de um jogo para edição
typedef struct
{
    char name[151];
    char description[501];
    double price;
    int available;
    int category_id;
    int found;
} GameData;

static int edit_game_callback(void *data, int argc, char **argv, char **azColName)
{
    GameData *game = (GameData *)data;
    game->found = 1;
    strncpy(game->name, argv[0], sizeof(game->name) - 1);
    strncpy(game->description, argv[1], sizeof(game->description) - 1);
    game->price = atof(argv[2]);
    game->available = atoi(argv[3]);
    game->category_id = atoi(argv[4]);
    return 0;
}

void edit_game()
{
    int game_id;
    GameData game = {0};

    printf("\n--- EDITAR JOGO ---\n");
    list_games();

    char id_buffer[10];
    get_input("Digite o ID do jogo que deseja editar: ", id_buffer, sizeof(id_buffer));
    if (sscanf(id_buffer, "%d", &game_id) != 1)
    {
        printf("ID inválido.\n");
        return;
    }

    // 1. Buscar dados atuais do jogo
    char *sql = sqlite3_mprintf("SELECT nome, descricao, preco, disponivel, categoria_id FROM jogos WHERE id = %d;", game_id);
    execute_query(sql, edit_game_callback, &game);
    sqlite3_free(sql);

    if (!game.found)
    {
        printf("Jogo com ID %d não encontrado.\n", game_id);
        return;
    }

    // 2. Pedir novos dados ao usuário
    char name[151], description[501], price_str[20], cat_id_buffer[10], available_buffer[10];

    printf("Editando o jogo '%s'. Deixe em branco para não alterar.\n", game.name);

    printf("Novo nome (atual: %s): ", game.name);
    get_input("", name, sizeof(name));
    if (strlen(name) == 0)
        strcpy(name, game.name);

    printf("Nova descrição (atual: %s): ", game.description);
    get_input("", description, sizeof(description));
    if (strlen(description) == 0)
        strcpy(description, game.description);

    printf("Novo preço (atual: %.2f): ", game.price);
    get_input("", price_str, sizeof(price_str));
    double new_price = (strlen(price_str) > 0) ? atof(price_str) : game.price;

    printf("Nova categoria (ID atual: %d): ", game.category_id);
    get_input("", cat_id_buffer, sizeof(cat_id_buffer));
    int new_category_id = (strlen(cat_id_buffer) > 0) ? atoi(cat_id_buffer) : game.category_id;

    printf("Disponível (1=Sim, 0=Não) (atual: %d): ", game.available);
    get_input("", available_buffer, sizeof(available_buffer));
    int new_available = (strlen(available_buffer) > 0) ? atoi(available_buffer) : game.available;

    // 3. Montar e executar o UPDATE
    sql = sqlite3_mprintf(
        "UPDATE jogos SET nome = '%q', descricao = '%q', preco = %f, categoria_id = %d, disponivel = %d WHERE id = %d;",
        name, description, new_price, new_category_id, new_available, game_id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) == 0)
    {
        printf("Erro ao atualizar o jogo. Verifique se o novo ID de categoria é válido.\n");
    }
    else
    {
        printf("Jogo atualizado com sucesso!\n");
    }

    sqlite3_free(sql);
}

void delete_game()
{
    int game_id;

    printf("\n--- DELETAR JOGO ---\n");
    list_games();

    char id_buffer[10];
    get_input("Digite o ID do jogo que deseja deletar: ", id_buffer, sizeof(id_buffer));
    if (sscanf(id_buffer, "%d", &game_id) != 1)
    {
        printf("ID inválido.\n");
        return;
    }

    // Antes de deletar, seria bom confirmar com o usuário.
    char confirm[10];
    printf("Tem certeza que deseja deletar o jogo com ID %d? (s/n): ", game_id);
    get_input("", confirm, sizeof(confirm));

    if (strcmp(confirm, "s") != 0 && strcmp(confirm, "S") != 0)
    {
        printf("Deleção cancelada.\n");
        return;
    }

    char *sql = sqlite3_mprintf("DELETE FROM jogos WHERE id = %d;", game_id);
    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) == 0)
    {
        printf("Jogo deletado com sucesso!\n");
    }
    else
    {
        // A falha pode ocorrer por causa de foreign keys em outras tabelas (transacoes, etc)
        printf("Erro ao deletar jogo. Verifique se o ID existe e se o jogo não está em transações, carrinhos, etc.\n");
    }

    sqlite3_free(sql);
}

void list_related_games();
void add_related_game();
void remove_related_game();

void manage_related_games()
{
    int option = -1;

    while (option != 0)
    {
        printf("\n--- GERENCIAR JOGOS RELACIONADOS ---\n");
        printf("1. Listar Jogos Relacionados a um Jogo\n");
        printf("2. Adicionar Relação entre Jogos\n");
        printf("3. Remover Relação entre Jogos\n");
        printf("0. Voltar\n");
        printf("-------------------------------------\n");

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            list_related_games();
            break;
        case 2:
            add_related_game();
            break;
        case 3:
            remove_related_game();
            break;
        case 0:
            printf("Voltando...\n");
            break;
        default:
            printf("Opção inválida!\n");
            break;
        }
    }
}

// Callback para listar jogos relacionados
static int list_related_callback(void *data, int argc, char **argv, char **azColName)
{
    // argv[0] = related_game.id, argv[1] = related_game.nome
    printf(" -> ID: %s, Nome: %s\n", argv[0], argv[1]);
    return 0;
}

void list_related_games()
{
    int game_id;
    printf("\n--- LISTAR JOGOS RELACIONADOS ---\n");
    list_games();

    char id_buffer[10];
    get_input("Digite o ID do jogo para ver suas relações: ", id_buffer, sizeof(id_buffer));
    if (sscanf(id_buffer, "%d", &game_id) != 1)
    {
        printf("ID inválido.\n");
        return;
    }

    printf("\nJogos relacionados ao jogo com ID %d:\n", game_id);

    char *sql = sqlite3_mprintf(
        "SELECT j2.id, j2.nome FROM jogos_relacionados jr "
        "JOIN jogos j2 ON jr.relacionado_id = j2.id "
        "WHERE jr.jogo_id = %d;",
        game_id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    execute_query(sql, list_related_callback, NULL);
    sqlite3_free(sql);
    printf("-------------------------------------\n");
}

void add_related_game()
{
    int game_id, related_id;
    printf("\n--- ADICIONAR JOGO RELACIONADO ---\n");
    list_games();

    char id_buffer1[10], id_buffer2[10];
    get_input("Digite o ID do jogo principal (ex: o jogo que está sendo visto): ", id_buffer1, sizeof(id_buffer1));
    if (sscanf(id_buffer1, "%d", &game_id) != 1)
    {
        printf("ID inválido.\n");
        return;
    }

    get_input("Digite o ID do jogo que será relacionado a ele: ", id_buffer2, sizeof(id_buffer2));
    if (sscanf(id_buffer2, "%d", &related_id) != 1)
    {
        printf("ID inválido.\n");
        return;
    }

    if (game_id == related_id)
    {
        printf("Um jogo não pode ser relacionado a si mesmo.\n");
        return;
    }

    // Inserir a relação nos dois sentidos para ser bidirecional
    char *sql1 = sqlite3_mprintf("INSERT INTO jogos_relacionados (jogo_id, relacionado_id) VALUES (%d, %d);", game_id, related_id);
    char *sql2 = sqlite3_mprintf("INSERT INTO jogos_relacionados (jogo_id, relacionado_id) VALUES (%d, %d);", related_id, game_id);

    if (!sql1 || !sql2)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        sqlite3_free(sql1);
        sqlite3_free(sql2);
        return;
    }

    int success1 = execute_non_query(sql1);
    int success2 = execute_non_query(sql2);

    if (success1 == 0 && success2 == 0)
    {
        printf("Relação adicionada com sucesso!\n");
    }
    else
    {
        printf("Erro ao adicionar relação. Verifique se os IDs são válidos e se a relação já não existe.\n");
    }

    sqlite3_free(sql1);
    sqlite3_free(sql2);
}

void remove_related_game()
{
    int game_id, related_id;
    printf("\n--- REMOVER JOGO RELACIONADO ---\n");
    list_games();

    char id_buffer1[10], id_buffer2[10];
    get_input("Digite o ID do jogo principal: ", id_buffer1, sizeof(id_buffer1));
    if (sscanf(id_buffer1, "%d", &game_id) != 1)
    {
        printf("ID inválido.\n");
        return;
    }

    // Listar os jogos relacionados a este para facilitar
    list_related_games();

    get_input("Digite o ID do jogo relacionado que deseja remover: ", id_buffer2, sizeof(id_buffer2));
    if (sscanf(id_buffer2, "%d", &related_id) != 1)
    {
        printf("ID inválido.\n");
        return;
    }

    // Remover a relação nos dois sentidos
    char *sql1 = sqlite3_mprintf("DELETE FROM jogos_relacionados WHERE jogo_id = %d AND relacionado_id = %d;", game_id, related_id);
    char *sql2 = sqlite3_mprintf("DELETE FROM jogos_relacionados WHERE jogo_id = %d AND relacionado_id = %d;", related_id, game_id);

    if (!sql1 || !sql2)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        sqlite3_free(sql1);
        sqlite3_free(sql2);
        return;
    }

    int success1 = execute_non_query(sql1);
    int success2 = execute_non_query(sql2);

    if (success1 == 0 && success2 == 0)
    {
        printf("Relação removida com sucesso!\n");
    }
    else
    {
        printf("Erro ao remover relação. Verifique se os IDs são válidos.\n");
    }

    sqlite3_free(sql1);
    sqlite3_free(sql2);
}
