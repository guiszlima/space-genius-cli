#include "BalanceController.h"
#include "../Auth/Auth.h"
#include "../../Utils/Input.h"
#include "../../Utils/Colors.h"
#include "../../Repository/Repository.h"
#include "../Transaction/TransactionController.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

void add_money();

void show_balance_menu()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf(C_WARN "Você não está logado." C_RESET "\n");
        return;
    }

    int option = -1;
    while (option != 0)
    {
        printf("\n" C_TITLE "--- SALDO E TRANSAÇÕES ---" C_RESET "\n");
        printf("%sSeu saldo atual: R$%.2f%s\n", C_INFO, user->saldo, C_RESET);
        printf(C_OPTION "----------------------------" C_RESET "\n");
        printf(C_OPTION "1. Adicionar Dinheiro" C_RESET "\n");
        printf(C_OPTION "2. Ver Histórico de Transações" C_RESET "\n");
        printf(C_OPTION "0. Voltar" C_RESET "\n");
        printf(C_OPTION "----------------------------" C_RESET "\n");

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
            printf(C_INFO "Voltando ao menu principal..." C_RESET "\n");
            break;
        default:
            printf(C_WARN "Opção inválida." C_RESET "\n");
            break;
        }
    }
}

void add_money()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf(C_WARN "Você precisa estar logado para adicionar dinheiro." C_RESET "\n");
        return;
    }

    char amount_buffer[20];
    double amount;

    printf("\n" C_TITLE "--- ADICIONAR DINHEIRO ---" C_RESET "\n");
    get_input(C_INFO "Digite o valor a ser adicionado: R$" C_RESET, amount_buffer, sizeof(amount_buffer));
    if (sscanf(amount_buffer, "%lf", &amount) != 1 || amount <= 0)
    {
        printf(C_WARN "Valor inválido. Digite um número positivo." C_RESET "\n");
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE usuarios SET saldo = saldo + %.2f WHERE id = %d;", amount, user->id);
    if (!sql)
    {
        fprintf(stderr, C_ERROR "Erro ao alocar memória para o SQL." C_RESET "\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        user->saldo += amount;
        printf(C_SUCCESS "R$%.2f adicionado à sua conta. Novo saldo: R$%.2f" C_RESET "\n", amount, user->saldo);
    }
    else
    {
        printf(C_ERROR "Erro ao adicionar dinheiro." C_RESET "\n");
    }

    sqlite3_free(sql);
}
