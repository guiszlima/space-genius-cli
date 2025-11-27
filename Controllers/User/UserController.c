#include "UserController.h"
#include "../../Repository/Repository.h"
#include "../Auth/Auth.h"
#include "../../Utils/Input.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

int count_rows_callback(void *data, int argc, char **argv, char **colNames);
void register_user_flow()
{
    char username[101];
    char email[151];
    char password[101];

    printf("\n%s--- CADASTRO DE NOVO USUÁRIO ---%s\n", C_TITLE, C_RESET);

    int tabela_vazia = 0;
    int row_count = 0;

    char *sql_check = sqlite3_mprintf("SELECT 1 FROM usuarios LIMIT 1;");

    execute_query(sql_check, count_rows_callback, &row_count);
    sqlite3_free(sql_check);

    if (row_count == 0)
    {
        tabela_vazia = 1;
        printf("\n%s-- PRIMEIRO REGISTRO: ADICIONAR USUÁRIO ADMINISTRADOR --%s\n\n", C_WARN, C_RESET);
    }

    get_input("Digite o nome de usuário: ", username, sizeof(username));
    get_input("Digite o e-mail: ", email, sizeof(email));
    get_input("Digite a senha: ", password, sizeof(password));

    if (strlen(username) == 0 || strlen(email) == 0 || strlen(password) == 0)
    {
        printf("%sTodos os campos são obrigatórios!%s\n", C_WARN, C_RESET);
        return;
    }

    char *sql = sqlite3_mprintf(
        "INSERT INTO usuarios (username, email, senha, admin) VALUES ('%q', '%q', '%q', %d);",
        username, email, password,
        tabela_vazia ? 1 : 0);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        if (tabela_vazia)
            printf("%sAdministrador '%s' cadastrado com sucesso!%s\n", C_SUCCESS, username, C_RESET);
        else
            printf("%sUsuário '%s' cadastrado com sucesso!%s\n", C_SUCCESS, username, C_RESET);
    }
    else
    {
        printf("%sErro ao cadastrar usuário. O username ou e-mail pode já existir.%s\n", C_ERROR, C_RESET);
    }

    sqlite3_free(sql);
}

typedef struct
{
    int id;
    char username[101];
    double saldo;
    int found;
} LoginData;

int count_rows_callback(void *data, int argc, char **argv, char **colNames)
{
    int *count = (int *)data;
    (*count)++;
    return 0;
}

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

    printf("\n%s--- LOGIN --- %s\n", C_TITLE, C_RESET);
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

        auth_set_usuario_logado(login_attempt.id, login_attempt.username, login_attempt.saldo);
        printf("%sLogin realizado com sucesso! Bem-vindo, %s!%s\n", C_SUCCESS, login_attempt.username, C_RESET);

        char *ip = "127.0.0.1";
        char *agent = "CLI-App/1.0";
        sql = sqlite3_mprintf("INSERT INTO login_historico (usuario_id, ip, agente_usuario) VALUES (%d, '%q', '%q');", login_attempt.id, ip, agent);

        if (sql)
        {
            execute_non_query(sql);
            sqlite3_free(sql);
        }

        int cart_found = 0;
        sql = sqlite3_mprintf("SELECT id FROM carrinhos WHERE usuario_id = %d;", login_attempt.id);
        execute_query(sql, cart_exists_callback, &cart_found);
        sqlite3_free(sql);

        if (!cart_found)
        {
            sql = sqlite3_mprintf("INSERT INTO carrinhos (usuario_id) VALUES (%d);", login_attempt.id);
            if (execute_non_query(sql) == 0)
            {
                printf("%sCarrinho de compras criado para o usuário.%s\n", C_SUCCESS, C_RESET);
            }
            sqlite3_free(sql);
        }
    }
    else
    {
        printf("%sUsuário ou senha inválidos.%s\n", C_ERROR, C_RESET);
    }
}
