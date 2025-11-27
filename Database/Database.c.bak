#include "Database.h"
#include <stdio.h>
#include <stdlib.h>

// Variável global para a conexão com o banco de dados
static sqlite3 *db = NULL;

// Função para conectar ao banco de dados
void db_connect()
{
    if (db)
    {
        return; // A conexão já existe
    }

    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Erro ao abrir o banco de dados: %s\n", sqlite3_errmsg(db));
        db_close(); // Tenta fechar para liberar recursos
        exit(1);    // Encerra a aplicação em caso de falha na conexão
    }

    // Habilita o suporte a chaves estrangeiras
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
}

// Função para fechar a conexão com o banco de dados
void db_close()
{
    if (db)
    {
        sqlite3_close(db);
        db = NULL;
    }
}

// Função para obter a instância da conexão
sqlite3 *get_db_connection()
{
    if (!db)
    {
        db_connect();
    }
    return db;
}
