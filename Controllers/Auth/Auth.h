#ifndef AUTH_H
#define AUTH_H

// Estrutura para manter os dados do usuário logado em memória
typedef struct
{
    int id;
    char username[101];
    double saldo;
} UsuarioLogado;

// Funções para gerenciar o estado de autenticação
void auth_set_usuario_logado(int id, const char *username, double saldo);
void auth_logout();
UsuarioLogado *auth_get_usuario_logado();
int auth_is_logged_in();

#endif // AUTH_H
