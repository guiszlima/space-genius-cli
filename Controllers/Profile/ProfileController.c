#include "ProfileController.h"
#include "../Auth/Auth.h"
#include "../../Repository/Repository.h"
#include "../../Utils/Input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

void editar_email();
void alterar_senha();
void ver_jogos_comprados();

void view_my_profile()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("%sVocê não está logado.%s\n", C_WARN, C_RESET);
        return;
    }

    printf("\n%s--- SEU PERFIL --- %s\n", C_TITLE, C_RESET);
    printf("%sID:%s %d\n", C_OPTION, C_RESET, user->id);
    printf("%sUsername:%s %s\n", C_OPTION, C_RESET, user->username);
    printf("%sSaldo:%s R$%.2f\n", C_OPTION, C_RESET, user->saldo);
    printf("%s-------------------%s\n", C_TITLE, C_RESET);
}

void show_profile_menu()
{
    int option = -1;

    while (option != 0)
    {
        view_my_profile();
        printf("\n%s--- MENU DO PERFIL --- %s\n", C_TITLE, C_RESET);
        printf("%s1. Editar E-mail%s\n", C_OPTION, C_RESET);
        printf("%s2. Alterar Senha%s\n", C_OPTION, C_RESET);
        printf("%s3. Mostrar jogos comprados%s\n", C_OPTION, C_RESET);
        printf("%s0. Voltar ao Menu Principal%s\n", C_OPTION, C_RESET);
        printf("%s------------------------%s\n", C_TITLE, C_RESET);

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            editar_email();
            break;
        case 2:
            alterar_senha();
            break;
        case 3:
            ver_jogos_comprados();
            break;
        case 0:
            printf("%sVoltando ao menu principal...%s\n", C_INFO, C_RESET);
            break;
        default:
            printf("%sOpção inválida!%s\n", C_WARN, C_RESET);
            break;
        }
    }
}

void editar_email()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("%sErro: Você precisa estar logado para editar seu e-mail.%s\n", C_ERROR, C_RESET);
        return;
    }

    char new_email[151];
    printf("\n%s--- EDITAR E-MAIL --- %s\n", C_TITLE, C_RESET);
    get_input("Digite o novo e-mail: ", new_email, sizeof(new_email));

    if (strlen(new_email) == 0)
    {
        printf("%sO e-mail não pode ser vazio.%s\n", C_WARN, C_RESET);
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE usuarios SET email = '%q' WHERE id = %d;", new_email, user->id);
    if (!sql)
    {
        fprintf(stderr, "%sErro ao alocar memória para o SQL.%s\n", C_ERROR, C_RESET);
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("%sE-mail atualizado com sucesso!%s\n", C_SUCCESS, C_RESET);
    }
    else
    {
        printf("%sErro ao atualizar o e-mail. O e-mail pode já estar em uso por outra conta.%s\n", C_ERROR, C_RESET);
    }

    sqlite3_free(sql);
}

void alterar_senha()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("%sErro: Você precisa estar logado para alterar sua senha.%s\n", C_ERROR, C_RESET);
        return;
    }

    char new_password[101];
    printf("\n%s--- ALTERAR SENHA --- %s\n", C_TITLE, C_RESET);
    get_input("Digite a nova senha: ", new_password, sizeof(new_password));

    if (strlen(new_password) == 0)
    {
        printf("%sA senha não pode ser vazia.%s\n", C_WARN, C_RESET);
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE usuarios SET senha = '%q' WHERE id = %d;", new_password, user->id);
    if (!sql)
    {
        fprintf(stderr, "%sErro ao alocar memória para o SQL.%s\n", C_ERROR, C_RESET);
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("%sSenha alterada com sucesso!%s\n", C_SUCCESS, C_RESET);
    }
    else
    {
        printf("%sErro ao alterar a senha.%s\n", C_ERROR, C_RESET);
    }

    sqlite3_free(sql);
}
int list_jogos_callback(void *data, int argc, char **argv, char **colNames)
{
    printf("%-5s | %-25s | %-40s | %-20s\n",
           argv[0] ? argv[0] : "",
           argv[1] ? argv[1] : "",
           argv[2] ? argv[2] : "",
           argv[3] ? argv[3] : "");

    return 0;
}

void ver_jogos_comprados()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você precisa estar logado para ver seus jogos comprados.\n");
        return;
    }

    printf("\n--- JOGOS COMPRADOS ---\n");

    char *sql = sqlite3_mprintf(
        "SELECT j.id, j.nome, j.descricao, c.nome AS categoria "
        "FROM jogos j "
        "JOIN categorias c ON j.categoria_id = c.id "
        "JOIN transacoes t ON t.jogo_id = j.id "
        "WHERE t.usuario_id = %d "
        "ORDER BY j.nome ASC;",
        user->id);

    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    printf("\n%-5s | %-25s | %-40s | %-20s\n",
           "ID", "Nome", "Descrição", "Categoria");
    printf("----------------------------------------------------------------------------------------------\n");

    execute_query(sql, list_jogos_callback, NULL);

    printf("----------------------------------------------------------------------------------------------\n");

    sqlite3_free(sql);
}
