#include "ProfileController.h"
#include "../Auth/Auth.h"
#include "../Repository/Repository.h"
#include "../Utils/Input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// Protótipos para as funções de edição
void editar_email();
void alterar_senha();
void ver_jogos_comprados();
// Função para exibir as informações do perfil do usuário
void view_my_profile()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Você não está logado.\n");
        return;
    }

    // Em um caso real, buscaríamos o email do banco, pois não está no struct Auth.
    printf("\n--- SEU PERFIL ---\n");
    printf("ID: %d\n", user->id);
    printf("Username: %s\n", user->username);
    printf("Saldo: R$%.2f\n", user->saldo);
    printf("-------------------\n");
}

void show_profile_menu()
{
    int option = -1;

    while (option != 0)
    {
        view_my_profile(); // Mostra o perfil atual
        printf("\n--- MENU DO PERFIL ---\n");
        printf("1. Editar E-mail\n");
        printf("2. Alterar Senha\n");
        printf("3. Mostrar jogos comprados\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("------------------------\n");

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
            printf("Voltando ao menu principal...\n");
            break;
        default:
            printf("Opção inválida!\n");
            break;
        }
    }
}

// Placeholders para as funções de edição
void editar_email()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Erro: Você precisa estar logado para editar seu e-mail.\n");
        return;
    }

    char new_email[151];
    printf("\n--- EDITAR E-MAIL ---\n");
    get_input("Digite o novo e-mail: ", new_email, sizeof(new_email));

    if (strlen(new_email) == 0)
    {
        printf("O e-mail não pode ser vazio.\n");
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE usuarios SET email = '%q' WHERE id = %d;", new_email, user->id);
    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("E-mail atualizado com sucesso!\n");
    }
    else
    {
        printf("Erro ao atualizar o e-mail. O e-mail pode já estar em uso por outra conta.\n");
    }

    sqlite3_free(sql);
}

void alterar_senha()
{
    UsuarioLogado *user = auth_get_usuario_logado();
    if (!user)
    {
        printf("Erro: Você precisa estar logado para alterar sua senha.\n");
        return;
    }

    char new_password[101];
    printf("\n--- ALTERAR SENHA ---\n");
    get_input("Digite a nova senha: ", new_password, sizeof(new_password));

    if (strlen(new_password) == 0)
    {
        printf("A senha não pode ser vazia.\n");
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE usuarios SET senha = '%q' WHERE id = %d;", new_password, user->id);
    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) > 0)
    {
        printf("Senha alterada com sucesso!\n");
    }
    else
    {
        printf("Erro ao alterar a senha.\n");
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
