# Space Genius — Play Store CLI

Projeto do 2º semestre de 2026 para a disciplina da UNINOVE, desenvolvido pela empresa fictícia "Space Genius".

## Descrição

Aplicação em C que simula uma loja de jogos (CLI). Possui cadastro/login de usuários, carrinho, lista de desejos, avaliações, gerenciamento de saldo e transações. Há uma seção administrativa que só fica disponível quando o campo `admin` na tabela `usuarios` estiver com valor `1`.

Funcionalidades principais:

- Registro e login de usuários
- Menu principal com opções: Perfil, Carrinho, Wishlist, Avaliações, Loja, Saldo/Transações, Menu Admin (apenas para administradores)
- Administração protegida por flag `admin` no banco de dados
- Migrações de esquema via `Migrate/migrate.c`
- Persistência em SQLite (`db.sqlite`)

## Requisitos

- gcc (Linux) ou MSYS2/MinGW (Windows)
- sqlite3 e headers de desenvolvimento (libsqlite3-dev no Linux / mingw-w64-sqlite3 no MSYS2)
- Make (opcional)

## Como compilar (Linux)

No Linux, a partir da raiz do projeto:

```bash
gcc -I. $(find . -name '*.c') -o app -lsqlite3
```

Observação: certifique-se de ter instalado o pacote de desenvolvimento do sqlite3 (por exemplo `sudo apt install libsqlite3-dev`).

## Como compilar (Windows)

Opções recomendadas:

1. WSL (Windows Subsystem for Linux)

- Instale e abra o WSL (Ubuntu etc.) e execute exatamente o mesmo comando do Linux acima:

```bash
gcc -I. $(find . -name '*.c') -o app -lsqlite3
```

2. MSYS2 / MinGW-w64 (compilar nativo .exe)

- Abra o shell MINGW64 e instale as ferramentas:

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-sqlite3
```

- Compile no MINGW64 shell:

```bash
gcc -I. $(find . -name '*.c') -o app.exe -lsqlite3
```

- Execute no MINGW64 shell:

```bash
./app.exe
```

- Ou no PowerShell/CMD:

```powershell
.\app.exe
```

3. Cross-compile a partir do Linux (avançado)

- Exige mingw-w64 e bibliotecas do sqlite compiladas para Windows; pode haver ajustes adicionais de link.

## Como executar

No Linux/WSL:

```bash
./app
```

No Windows (MSYS2) ou executável gerado:

```bash
./app.exe
# ou no PowerShell/CMD
.\app.exe
```

## Notas

- O arquivo de banco local `db.sqlite` está ignorado por `.gitignore`. Não versionar o banco local.
- Para reexecutar migrações: veja `Migrate/migrate.c`.
- As cores do terminal são usadas para melhorar a estética; se houver problemas, execute em um terminal compatível (Linux, WSL, MINGW64).
