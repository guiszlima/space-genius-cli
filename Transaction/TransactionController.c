#include "TransactionController.h"
#include "../Auth/Auth.h"
#include "../Repository/Repository.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// Struct para guardar os itens do carrinho para a transação
typedef struct CartItem
{
    int jogo_id;
    int quantidade;
    double preco;
    struct CartItem *next;
} CartItem;

// Callback para buscar os itens do carrinho
static int get_cart_items_callback(void *data, int argc, char **argv, char **azColName)
{
    CartItem **head = (CartItem **)data;
    CartItem *new_item = (CartItem *)malloc(sizeof(CartItem));
    if (!new_item)
    {
        fprintf(stderr, "Falha ao alocar memória para item do carrinho.\n");
        return 1; // Para a execução
    }

    // argv[0]: jogo_id, argv[1]: quantidade, argv[2]: preco
    new_item->jogo_id = atoi(argv[0]);
    new_item->quantidade = atoi(argv[1]);
    new_item->preco = atof(argv[2]);
    new_item->next = *head;
    *head = new_item;

    return 0;
}

void finalize_purchase()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você precisa estar logado para finalizar a compra.\n");
        return;
    }

    // 1. Buscar todos os itens do carrinho
    CartItem *items_head = NULL;
    char *sql = sqlite3_mprintf(
        "SELECT ci.jogo_id, ci.quantidade, j.preco FROM carrinho_itens ci "
        "JOIN jogos j ON ci.jogo_id = j.id "
        "JOIN carrinhos c ON ci.carrinho_id = c.id "
        "WHERE c.usuario_id = %d;",
        user->id);
    execute_query(sql, get_cart_items_callback, &items_head);
    sqlite3_free(sql);

    if (items_head == NULL)
    {
        printf("Seu carrinho está vazio.\n");
        return;
    }

    // 2. Calcular o total e verificar o saldo
    double total_price = 0;
    for (CartItem *item = items_head; item != NULL; item = item->next)
    {
        total_price += item->quantidade * item->preco;
    }

    if (user->saldo < total_price)
    {
        printf("Saldo insuficiente! Saldo atual: R$%.2f, Total da compra: R$%.2f\n", user->saldo, total_price);
        // Limpar a lista de itens
        while (items_head != NULL)
        {
            CartItem *temp = items_head;
            items_head = items_head->next;
            free(temp);
        }
        return;
    }

    // 3. Iniciar transação no banco de dados
    printf("Iniciando a transação...\n");
    execute_non_query("BEGIN TRANSACTION;");

    // 4. Descontar o saldo do usuário
    double new_saldo = user->saldo - total_price;
    sql = sqlite3_mprintf("UPDATE usuarios SET saldo = %.2f WHERE id = %d;", new_saldo, user->id);
    if (execute_non_query(sql) <= 0)
    {
        printf("Erro ao atualizar o saldo do usuário. Revertendo transação.\n");
        execute_non_query("ROLLBACK;");
        sqlite3_free(sql);
        return;
    }
    sqlite3_free(sql);

    // 5. Registrar cada compra na tabela de transações
    for (CartItem *item = items_head; item != NULL; item = item->next)
    {
        sql = sqlite3_mprintf(
            "INSERT INTO transacoes (usuario_id, jogo_id, tipo, valor) VALUES (%d, %d, 'compra', %f);",
            user->id, item->jogo_id, item->preco * item->quantidade);
        if (execute_non_query(sql) <= 0)
        {
            printf("Erro ao registrar transação para o jogo ID %d. Revertendo.\n", item->jogo_id);
            execute_non_query("ROLLBACK;");
            sqlite3_free(sql);
            return;
        }
        sqlite3_free(sql);
    }

    // 6. Limpar o carrinho
    sql = sqlite3_mprintf("DELETE FROM carrinho_itens WHERE carrinho_id IN (SELECT id FROM carrinhos WHERE usuario_id = %d);", user->id);
    if (execute_non_query(sql) < 0)
    {
        printf("Erro ao limpar o carrinho. Revertendo transação.\n");
        execute_non_query("ROLLBACK;");
        sqlite3_free(sql);
        return;
    }
    sqlite3_free(sql);

    // 7. Commit da transação
    if (execute_non_query("COMMIT;") >= 0)
    {
        printf("Compra finalizada com sucesso!\n");
        // Atualiza o saldo do usuário em memória
        user->saldo = new_saldo;
    }
    else
    {
        printf("Erro final ao commitar a transação. Tentando reverter.\n");
        execute_non_query("ROLLBACK;");
    }

    // Limpar a lista de itens da memória
    while (items_head != NULL)
    {
        CartItem *temp = items_head;
        items_head = items_head->next;
        free(temp);
    }
}

// Callback para listar as transações
static int list_transactions_callback(void *data, int argc, char **argv, char **azColName)
{
    // argv[0]: id, argv[1]: jogo_nome, argv[2]: tipo, argv[3]: valor, argv[4]: data
    printf("%-5s | %-30s | %-10s | R$ %-10s | %-20s\n",
           argv[0], argv[1], argv[2], argv[3], argv[4]);
    return 0;
}

void view_user_transactions()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você precisa estar logado para ver suas transações.\n");
        return;
    }

    printf("\n--- SUAS TRANSAÇÕES ---\n");

    char *sql = sqlite3_mprintf(
        "SELECT t.id, j.nome, t.tipo, t.valor, t.realizado_em "
        "FROM transacoes t JOIN jogos j ON t.jogo_id = j.id "
        "WHERE t.usuario_id = %d ORDER BY t.realizado_em DESC;",
        user->id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    printf("\n%-5s | %-30s | %-10s | %-11s | %-20s\n", "ID", "Jogo", "Tipo", "Valor", "Data");
    printf("------------------------------------------------------------------------------------------\n");
    execute_query(sql, list_transactions_callback, NULL);
    printf("------------------------------------------------------------------------------------------\n");

    sqlite3_free(sql);
}
