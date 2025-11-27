#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>


#define DB_PATH "db.sqlite"


sqlite3 *get_db_connection();
void db_connect();
void db_close();

#endif 
