#include "CartController.h"
#include "../../Utils/Input.h"
#include "../../Repository/Repository.h"
#include "../Auth/Auth.h"
#include "../Transaction/TransactionController.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Database/Database.h"
#include <sqlite3.h>

// Protótipos
void add_to_cart();
void list_cart_items();
void remove_from_cart();
void clear_cart();

// Callback para listar itens do carrinho
static int list_cart_callback(void *data, int argc, char **argv, char **azColName)
{
    // data: ponteiro para o total
    double *total = (double *)data;
    // argv[0]: item_id, argv[1]: jogo_nome, argv[2]: quantidade, argv[3]: preco_unitario
    int quantidade = atoi(argv[2]);
    double preco = atof(argv[3]);
    double subtotal = quantidade * preco;
    *total += subtotal;

    printf("%-5s | %-30s | %-10d | R$ %-10.2f | R$ %-10.2f\n",
           argv[0], argv[1], quantidade, preco, subtotal);

    return 0;
}

void show_cart_menu()
{
    int option = -1;

    while (option != 0)
    {
        printf("\n--- SEU CARRINHO DE COMPRAS ---\n");
        list_cart_items(); // Sempre lista os itens ao entrar no menu
        printf("---------------------------------\n");
        printf("1. Adicionar Jogo ao Carrinho\n");
        printf("2. Remover Jogo do Carrinho\n");
        printf("3. Limpar Carrinho\n");
        printf("4. Finalizar Compra\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("---------------------------------\n");

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            add_to_cart();
            break;
        case 2:
            remove_from_cart();
            break;
        case 3:
            clear_cart();
            break;
        case 4:
            finalize_purchase();
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

void list_cart_items()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você não está logado.\n");
        return;
    }

    double total = 0.0;

    char *sql = sqlite3_mprintf(
        "SELECT ci.id, j.nome, ci.quantidade, j.preco "
        "FROM carrinho_itens ci "
        "JOIN jogos j ON ci.jogo_id = j.id "
        "JOIN carrinhos c ON ci.carrinho_id = c.id "
        "WHERE c.usuario_id = %d;",
        user->id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    printf("\n%-5s | %-30s | %-10s | %-11s | %-11s\n", "ID", "Jogo", "Quantidade", "Preço Unit.", "Subtotal");
    printf("----------------------------------------------------------------------------------------\n");

    execute_query(sql, list_cart_callback, &total);

    printf("----------------------------------------------------------------------------------------\n");
    printf("TOTAL DO CARRINHO: R$ %.2f\n", total);

    sqlite3_free(sql);
}

// Callback para pegar o ID do carrinho e o ID de um item existente
static int get_cart_info_callback(void *data, int argc, char **argv, char **azColName)
{
    int *results = (int *)data;
    // results[0] = cart_id, results[1] = item_id
    if (strcmp(azColName[0], "carrinho_id") == 0)
    {
        results[0] = atoi(argv[0]);
    }
    if (argc > 1 && strcmp(azColName[1], "item_id") == 0)
    {
        results[1] = atoi(argv[1]);
    }
    return 0;
}

void add_to_cart()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
        return;

    // Reutilizar a função de listar jogos do GameController
    // Em um projeto real, a função de listar jogos seria mais genérica
    extern void list_games();
    list_games();

    char id_buffer[10], qty_buffer[10];
    int game_id, quantity;

    get_input("Digite o ID do jogo para adicionar ao carrinho: ", id_buffer, sizeof(id_buffer));
    if (sscanf(id_buffer, "%d", &game_id) != 1)
    {
        printf("ID de jogo inválido.\n");
        return;
    }

    get_input("Digite a quantidade: ", qty_buffer, sizeof(qty_buffer));
    if (sscanf(qty_buffer, "%d", &quantity) != 1 || quantity <= 0)
    {
        printf("Quantidade inválida.\n");
        return;
    }

    // 1. Obter o ID do carrinho do usuário
    int cart_info[2] = {-1, -1}; // cart_id, item_id
    char *sql = sqlite3_mprintf("SELECT id AS carrinho_id FROM carrinhos WHERE usuario_id = %d;", user->id);
    execute_query(sql, get_cart_info_callback, cart_info);
    sqlite3_free(sql);

    if (cart_info[0] == -1)
    {
        printf("Erro: Carrinho não encontrado para este usuário.\n");
        return;
    }
    int cart_id = cart_info[0];

    // 2. Verificar se o item já existe no carrinho
    sql = sqlite3_mprintf("SELECT id AS item_id FROM carrinho_itens WHERE carrinho_id = %d AND jogo_id = %d;", cart_id, game_id);
    cart_info[1] = -1; // Reset item_id
    execute_query(sql, get_cart_info_callback, cart_info);
    sqlite3_free(sql);

    int item_id = cart_info[1];

    if (item_id != -1)
    {
        // Item existe, então UPDATE
        sql = sqlite3_mprintf("UPDATE carrinho_itens SET quantidade = quantidade + %d WHERE id = %d;", quantity, item_id);
    }
    else
    {
        // Item não existe, então INSERT
        sql = sqlite3_mprintf("INSERT INTO carrinho_itens (carrinho_id, jogo_id, quantidade) VALUES (%d, %d, %d);", cart_id, game_id, quantity);
    }

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) == 0)
    {
        printf("Erro ao adicionar jogo ao carrinho. Verifique se o ID do jogo é válido.\n");
    }
    else
    {
        printf("Jogo adicionado/atualizado no carrinho com sucesso!\n");
    }

    sqlite3_free(sql);
}

void remove_from_cart()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
        return;

    char item_id_buffer[10];
    int item_id;

    // A lista de itens já é mostrada no menu principal do carrinho
    get_input("Digite o ID do item do carrinho para remover: ", item_id_buffer, sizeof(item_id_buffer));
    if (sscanf(item_id_buffer, "%d", &item_id) != 1)
    {
        printf("ID de item inválido.\n");
        return;
    }

    // Precisamos garantir que o item pertence ao carrinho do usuário logado para segurança
    char *sql = sqlite3_mprintf(
        "DELETE FROM carrinho_itens WHERE id = %d AND carrinho_id IN (SELECT id FROM carrinhos WHERE usuario_id = %d);",
        item_id, user->id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) == 0)
    {
        // Checar se alguma linha foi afetada
        int changes = sqlite3_changes(get_db_connection());
        if (changes > 0)
        {
            printf("Item removido do carrinho com sucesso!\n");
        }
        else
        {
            printf("Nenhum item com esse ID foi encontrado no seu carrinho.\n");
        }
    }
    else
    {
        printf("Erro ao remover item do carrinho.\n");
    }

    sqlite3_free(sql);
}

void clear_cart()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
        return;

    char confirm[10];
    printf("Tem certeza que deseja limpar todos os itens do seu carrinho? (s/n): ");
    get_input("", confirm, sizeof(confirm));

    if (strcmp(confirm, "s") != 0 && strcmp(confirm, "S") != 0)
    {
        printf("Operação cancelada.\n");
        return;
    }

    // A subquery garante que estamos limpando o carrinho do usuário correto.
    char *sql = sqlite3_mprintf(
        "DELETE FROM carrinho_itens WHERE carrinho_id IN (SELECT id FROM carrinhos WHERE usuario_id = %d);",
        user->id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    int changes = execute_non_query(sql);
    sqlite3_free(sql);

    if (changes >= 0)
    {
        if (changes > 0)
        {
            printf("%d item(s) removidos do carrinho.\n", changes);
        }
        else
        {
            printf("O carrinho já estava vazio.\n");
        }
    }
    else
    {
        printf("Erro ao limpar o carrinho.\n");
    }
}
