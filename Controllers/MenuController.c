#include <stdio.h>
#include <stdlib.h>
#include "MenuController.h"
// Variável global simulando o estado de login
int logged_in = 0;

// Protótipos das funções que seriam implementadas
void login();
void register_user();
void inserir_dinheiro();
void comprar_jogo();
void reembolso();

void show_main_menu(void)
{
    int option;

    printf("\n===== PLAY STORE CLI =====\n");

    // Menu dinâmico
    if (!logged_in)
    {
        printf("1. Login\n");
        printf("2. Register\n");
        printf("0. Sair\n");
    }
    else
    {
        printf("1. Inserir dinheiro\n");
        printf("2. Comprar jogo\n");
        printf("3. Reembolso\n");
        printf("0. Sair\n");
    }

    printf("==========================\n");
    printf("Escolha uma opção: ");

    if (scanf("%d", &option) != 1)
    {
        while (getchar() != '\n')
            ; // limpa buffer
        printf("Opção inválida! Tente novamente.\n");
        return;
    }

    if (!logged_in)
    {
        switch (option)
        {
        case 1:
            login();
            break;
        case 2:
            register_user();
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
            inserir_dinheiro();
            break;
        case 2:
            comprar_jogo();
            break;
        case 3:
            reembolso();
            break;
        case 0:
            printf("Saindo...\n");
            exit(0);
        default:
            printf("Opção inválida! Tente novamente.\n");
            break;
        }
    }
}

// Funções simuladas
void login()
{
    printf("Login realizado com sucesso!\n");
    logged_in = 1;
}

void register_user()
{
    printf("Cadastro realizado com sucesso!\n");
    logged_in = 1;
}

void inserir_dinheiro() { printf("Inserir dinheiro\n"); }
void comprar_jogo() { printf("Comprar jogo\n"); }
void reembolso() { printf("Reembolso\n"); }
