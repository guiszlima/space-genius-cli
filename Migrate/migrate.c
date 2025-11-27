#include "migrate.h"
#include "../Repository/Repository.h"
#include <stdio.h>

void run_migrations(void)
{
    const char *sql =
        "CREATE TABLE IF NOT EXISTS usuarios ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "senha TEXT NOT NULL,"
        "email TEXT NOT NULL UNIQUE,"
        "saldo NUMERIC NOT NULL DEFAULT 0,"
        "criado_em DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "atualizado_em DATETIME"
        ");"

        "CREATE TABLE IF NOT EXISTS login_historico ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "usuario_id INTEGER NOT NULL,"
        "ip TEXT NOT NULL,"
        "agente_usuario TEXT,"
        "logado_em DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(usuario_id) REFERENCES usuarios(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS categorias ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nome TEXT NOT NULL UNIQUE"
        ");"

        "CREATE TABLE IF NOT EXISTS jogos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "categoria_id INTEGER NOT NULL,"
        "nome TEXT NOT NULL,"
        "descricao TEXT,"
        "preco NUMERIC NOT NULL,"
        "disponivel INTEGER NOT NULL DEFAULT 1,"
        "criado_em DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(categoria_id) REFERENCES categorias(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS jogos_relacionados ("
        "jogo_id INTEGER NOT NULL,"
        "relacionado_id INTEGER NOT NULL,"
        "PRIMARY KEY (jogo_id, relacionado_id),"
        "FOREIGN KEY(jogo_id) REFERENCES jogos(id),"
        "FOREIGN KEY(relacionado_id) REFERENCES jogos(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS transacoes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "usuario_id INTEGER NOT NULL,"
        "jogo_id INTEGER NOT NULL,"
        "tipo TEXT NOT NULL CHECK(tipo IN ('compra','reembolso','bonus')),"
        "valor NUMERIC NOT NULL,"
        "realizado_em DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(usuario_id) REFERENCES usuarios(id),"
        "FOREIGN KEY(jogo_id) REFERENCES jogos(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS avaliacoes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "usuario_id INTEGER NOT NULL,"
        "jogo_id INTEGER NOT NULL,"
        "nota INTEGER NOT NULL CHECK(nota BETWEEN 1 AND 5),"
        "comentario TEXT,"
        "criado_em DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(usuario_id) REFERENCES usuarios(id),"
        "FOREIGN KEY(jogo_id) REFERENCES jogos(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS carrinhos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "usuario_id INTEGER NOT NULL,"
        "criado_em DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(usuario_id) REFERENCES usuarios(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS carrinho_itens ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "carrinho_id INTEGER NOT NULL,"
        "jogo_id INTEGER NOT NULL,"
        "quantidade INTEGER NOT NULL DEFAULT 1,"
        "FOREIGN KEY(carrinho_id) REFERENCES carrinhos(id),"
        "FOREIGN KEY(jogo_id) REFERENCES jogos(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS lista_desejos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "usuario_id INTEGER NOT NULL,"
        "jogo_id INTEGER NOT NULL,"
        "adicionado_em DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "UNIQUE(usuario_id, jogo_id),"
        "FOREIGN KEY(usuario_id) REFERENCES usuarios(id),"
        "FOREIGN KEY(jogo_id) REFERENCES jogos(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS logs_admin ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "admin TEXT NOT NULL,"
        "acao TEXT NOT NULL,"
        "alvo TEXT,"
        "criado_em DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP"
        ");";

    if (execute_non_query(sql) >= 0)
    {
        printf("Banco de dados migrado com sucesso!\n");
    }
    else
    {
        fprintf(stderr, "Falha ao migrar o banco de dados.\n");
    }
}
