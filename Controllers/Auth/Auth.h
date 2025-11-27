#ifndef AUTH_H
#define AUTH_H


typedef struct
{
    int id;
    char username[101];
    double saldo;
} UsuarioLogado;


void auth_set_usuario_logado(int id, const char *username, double saldo);
void auth_logout();
UsuarioLogado *auth_get_usuario_logado();
int auth_is_logged_in();

#endif 
