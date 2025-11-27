#include "BalanceController.h"
#include "../Auth/Auth.h"
#include "../../Utils/Input.h"
#include "../Transaction/TransactionController.h"
#include <stdio.h>

void add_money();

void show_balance_menu()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você não está logado.\n");
        return;
    }

    int option = -1;
    while (option != 0)
    {
        printf("\n--- SALDO E TRANSAÇÕES ---\n");
        printf("Seu saldo atual: R$%.2f\n", user->saldo);
        printf("----------------------------\n");
        printf("1. Adicionar Dinheiro\n");
        printf("2. Ver Histórico de Transações\n");
        printf("0. Voltar\n");
        printf("----------------------------\n");

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            add_money();
            break;
        case 2:
            view_user_transactions();
            break;
        case 0:
            printf("Voltando ao menu principal...\n");
            break;
        default:
            printf("Opção inválida.\n");
            break;
        }
    }
}

#include "BalanceController.h"
#include "../Auth/Auth.h"
#include "../../Utils/Input.h"
#include "../../Repository/Repository.h"
#include "../Transaction/TransactionController.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

void add_money()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você precisa estar logado para adicionar dinheiro.\n");
        return;
    }

    char amount_buffer[20];
    double amount;

    printf("\n--- ADICIONAR DINHEIRO ---\n");
    get_input("Digite o valor a ser adicionado: R$", amount_buffer, sizeof(amount_buffer));
    if (sscanf(amount_buffer, "%lf", &amount) != 1 || amount <= 0)
    {
        printf("Valor inválido. Digite um número positivo.\n");
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE usuarios SET saldo = saldo + %.2f WHERE id = %d;", amount, user->id);
    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        user->saldo += amount; 
        printf("R$%.2f adicionado à sua conta. Novo saldo: R$%.2f\n", amount, user->saldo);
    }
    else
    {
        printf("Erro ao adicionar dinheiro.\n");
    }

    sqlite3_free(sql);
}
