#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <sqlite3.h>

// Tipo de função de callback para consultas que retornam múltiplos resultados.
// Esta função será chamada para cada linha do resultado.
typedef int (*query_callback)(void *data, int argc, char **argv, char **azColName);

/**
 * @brief Executa uma instrução SQL que não retorna dados (INSERT, UPDATE, DELETE).
 *
 * @param sql A string SQL a ser executada.
 * @return int Retorna o número de linhas afetadas em sucesso, -1 em falha.
 */
int execute_non_query(const char *sql);

/**
 * @brief Executa uma consulta SQL e processa os resultados com uma função de callback.
 *        Ideal para buscar múltiplos registros.
 *
 * @param sql A string SQL da consulta.
 * @param callback A função a ser chamada para cada linha de resultado.
 * @param data Ponteiro opcional para passar dados para a função de callback.
 * @return int Retorna 0 em sucesso, 1 em falha.
 */
int execute_query(const char *sql, query_callback callback, void *data);

#endif // REPOSITORY_H
