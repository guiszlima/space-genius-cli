#include <stdio.h>
#include <sqlite3.h>
#include "migrate.h"
void migrate(void)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;

    // Abre (ou cria) o banco de dados
    rc = sqlite3_open("db.sqlite", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Erro ao abrir banco: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Script SQL de criação das tabelas
    const char *sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password TEXT NOT NULL,"
        "balance REAL NOT NULL DEFAULT 0,"
        "datetime DATETIME NOT NULL"
        ");"

        "CREATE TABLE IF NOT EXISTS games ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "price REAL NOT NULL,"
        "available BOOLEAN NOT NULL DEFAULT 1,"
        "datetime DATETIME NOT NULL"
        ");"

        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "game_id INTEGER NOT NULL,"
        "type TEXT NOT NULL,"
        "amount REAL NOT NULL,"
        "datetime DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(game_id) REFERENCES games(id),"
        "FOREIGN KEY(user_id) REFERENCES users(id)"
        ");";

    // Executa o script SQL
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Erro ao criar tabelas: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }

    printf("Banco de dados 'db.sqlite' criado e migrado com sucesso!\n");

    sqlite3_close(db);
}
