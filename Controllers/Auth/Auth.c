#include "Auth.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../Utils/Colors.h"

static UsuarioLogado *g_usuario_logado = NULL;

/**
 * @brief Define o usuário que acabou de fazer login.
 *
 * @param id O ID do usuário.
 * @param username O nome de usuário.
 * @param saldo O saldo do usuário.

 */
void auth_set_usuario_logado(int id, const char *username, double saldo)
{

    if (g_usuario_logado != NULL)
    {
        free(g_usuario_logado);
    }

    g_usuario_logado = (UsuarioLogado *)malloc(sizeof(UsuarioLogado));
    if (g_usuario_logado == NULL)
    {
        fprintf(stderr, "Falha ao alocar memória para o usuário logado.\n");
        exit(1);
    }

    g_usuario_logado->id = id;
    strncpy(g_usuario_logado->username, username, sizeof(g_usuario_logado->username) - 1);
    g_usuario_logado->username[sizeof(g_usuario_logado->username) - 1] = '\0';
    g_usuario_logado->saldo = saldo;
}

/**
 * @brief Realiza o logout do usuário, limpando os dados da memória.
 */
void auth_logout()
{
    if (g_usuario_logado != NULL)
    {
        free(g_usuario_logado);
        g_usuario_logado = NULL;
        printf("%sLogout realizado com sucesso.%s\n", C_SUCCESS, C_RESET);
    }
}

/**
 * @brief Obtém um ponteiro para o struct do usuário logado.
 *
 * @return UsuarioLogado* Um ponteiro para o usuário logado ou NULL se ninguém estiver logado.
 */
UsuarioLogado *auth_get_usuario_logado()
{
    return g_usuario_logado;
}

/**
 * @brief Verifica se há um usuário logado.
 *
 * @return int Retorna 1 se houver um usuário logado, 0 caso contrário.
 */
int auth_is_logged_in()
{
    return g_usuario_logado != NULL;
}
