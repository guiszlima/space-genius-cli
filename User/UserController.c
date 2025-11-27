#include "UserController.h"
#include "../Repository/Repository.h"
#include "../Auth/Auth.h"
#include "../Utils/Input.h" // Incluindo o novo utilitário de input
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

void register_user_flow()
{
    char username[101];
    char email[151];
    char password[101];

    printf("\n--- CADASTRO DE NOVO USUÁRIO ---\n");

    // Coleta de dados
    get_input("Digite o nome de usuário: ", username, sizeof(username));
    get_input("Digite o e-mail: ", email, sizeof(email));
    get_input("Digite a senha: ", password, sizeof(password));

    // Validação simples
    if (strlen(username) == 0 || strlen(email) == 0 || strlen(password) == 0)
    {
        printf("Todos os campos são obrigatórios!\n");
        return;
    }

    // Monta o SQL de forma segura
    char *sql = sqlite3_mprintf("INSERT INTO usuarios (username, email, senha) VALUES ('%q', '%q', '%q');", username, email, password);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("Usuário '%s' cadastrado com sucesso!\n", username);
    }
    else
    {
        printf("Erro ao cadastrar usuário. O username ou e-mail pode já existir.\n");
    }

    sqlite3_free(sql);
}

// Struct para guardar os dados da tentativa de login
typedef struct
{
    int id;
    char username[101];
    double saldo;
    int found;
} LoginData;

// Callback para processar o resultado da consulta de login
static int login_callback(void *data, int argc, char **argv, char **azColName)
{
    LoginData *login_attempt = (LoginData *)data;
    login_attempt->found = 1;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(azColName[i], "id") == 0)
        {
            login_attempt->id = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "username") == 0)
        {
            strncpy(login_attempt->username, argv[i], sizeof(login_attempt->username) - 1);
        }
        else if (strcmp(azColName[i], "saldo") == 0)
        {
            login_attempt->saldo = atof(argv[i]);
        }
    }
    return 0;
}

// Callback para verificar a existência de um carrinho
static int cart_exists_callback(void *data, int argc, char **argv, char **azColName)
{
    int *found = (int *)data;
    *found = 1;
    return 0;
}

void login_user_flow()
{
    char username[101];
    char password[101];
    LoginData login_attempt = {0};

    printf("\n--- LOGIN ---\n");
    get_input("Usuário: ", username, sizeof(username));
    get_input("Senha: ", password, sizeof(password));

    char *sql = sqlite3_mprintf("SELECT id, username, saldo FROM usuarios WHERE username = '%q' AND senha = '%q';", username, password);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL de login.\n");
        return;
    }

    execute_query(sql, login_callback, &login_attempt);
    sqlite3_free(sql);

    if (login_attempt.found)
    {
        // Define o usuário logado globalmente
        auth_set_usuario_logado(login_attempt.id, login_attempt.username, login_attempt.saldo);
        printf("Login realizado com sucesso! Bem-vindo, %s!\n", login_attempt.username);

        // Insere no histórico de login
        char *ip = "127.0.0.1";
        char *agent = "CLI-App/1.0";
        sql = sqlite3_mprintf("INSERT INTO login_historico (usuario_id, ip, agente_usuario) VALUES (%d, '%q', '%q');", login_attempt.id, ip, agent);

        if (sql)
        {
            execute_non_query(sql);
            sqlite3_free(sql);
        }

        // Verifica e cria o carrinho se não existir
        int cart_found = 0;
        sql = sqlite3_mprintf("SELECT id FROM carrinhos WHERE usuario_id = %d;", login_attempt.id);
        execute_query(sql, cart_exists_callback, &cart_found);
        sqlite3_free(sql);

        if (!cart_found)
        {
            sql = sqlite3_mprintf("INSERT INTO carrinhos (usuario_id) VALUES (%d);", login_attempt.id);
            if (execute_non_query(sql) == 0)
            {
                printf("Carrinho de compras criado para o usuário.\n");
            }
            sqlite3_free(sql);
        }
    }
    else
    {
        printf("Usuário ou senha inválidos.\n");
    }
}