#include "Repository.h"
#include "../Database/Database.h"
#include <stdio.h>

/**
 * @brief Executa uma instrução SQL que não retorna dados (INSERT, UPDATE, DELETE).
 *
 * @param sql A string SQL a ser executada.
 * @return int Retorna o número de linhas afetadas em sucesso, -1 em falha.
 */
int execute_non_query(const char *sql)
{
    sqlite3 *db = get_db_connection();
    char *err_msg = 0;

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Erro no SQL (non-query): %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1; // Falha
    }

    return sqlite3_changes(db); // Sucesso, retorna o número de linhas afetadas
}

/**
 * @brief Executa uma consulta SQL e processa os resultados com uma função de callback.
 *
 * @param sql A string SQL da consulta.
 * @param callback A função a ser chamada para cada linha de resultado.
 * @param data Ponteiro opcional para passar dados para a função de callback.
 * @return int Retorna 0 em sucesso, 1 em falha.
 */
int execute_query(const char *sql, query_callback callback, void *data)
{
    sqlite3 *db = get_db_connection();
    char *err_msg = 0;

    int rc = sqlite3_exec(db, sql, callback, data, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Erro no SQL (query): %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1; // Falha
    }

    return 0; // Sucesso
}
