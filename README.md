# Space Genius — Play Store CLI

Projeto do 2º semestre de 2026 para a disciplina da UNINOVE, desenvolvido pela empresa fictícia "Space Genius".

## Descrição

Aplicação em C que simula uma loja de jogos (CLI). Possui cadastro/login de usuários, carrinho, lista de desejos, avaliações, gerenciamento de saldo e transações. Há uma seção administrativa que só fica disponível quando o campo `admin` na tabela `usuarios` estiver com valor `1`.

Funcionalidades principais

- Registro e login de usuários
- Menu principal com opções: Perfil, Carrinho, Wishlist, Avaliações, Loja, Saldo/Transações, Menu Admin (apenas para administradores)
- Administração protegida por flag `admin` no banco de dados
- Migrações de esquema via `Migrate/migrate.c`
- Persistência em SQLite (`db.sqlite`)

## Como compilar

No Linux, com o gcc instalado (e sqlite3 dev):

```bash
gcc -I. $(find . -name '*.c') -o app -lsqlite3
```

## Como executar

```bash
./app
```

Observações importantes

- O arquivo do banco local é `db.sqlite` e está listado em `.gitignore` para não ser enviado ao repositório.
- Migrações: a aplicação contém um módulo de migração (`Migrate`) que cria as tabelas necessárias; rode a aplicação para aplicar as migrações ou compile/execute `migrate.c` conforme a organização do projeto.
- Backup: durante algumas operações de refatoração, arquivos de backup com extensão `.bak` podem existir no repositório de trabalho.
- Cores: as strings de interface foram colorizadas usando `Utils/Colors.h` (macros ANSI) — isso é puramente estético.
