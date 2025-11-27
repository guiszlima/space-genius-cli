#include "Database.h"
#include <stdio.h>
#include <stdlib.h>


static sqlite3 *db = NULL;


void db_connect()
{
    if (db)
    {
        return; 
    }

    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Erro ao abrir o banco de dados: %s\n", sqlite3_errmsg(db));
        db_close(); 
        exit(1);    
    }

    
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
}


void db_close()
{
    if (db)
    {
        sqlite3_close(db);
        db = NULL;
    }
}


sqlite3 *get_db_connection()
{
    if (!db)
    {
        db_connect();
    }
    return db;
}
