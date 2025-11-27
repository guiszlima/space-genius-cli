#include "CategoryController.h"
#include "../Utils/Input.h"
#include "../Repository/Repository.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// Protótipos
void add_category();
void list_categories();
void edit_category();
void delete_category();

// Struct para guardar os dados de uma categoria
typedef struct
{
    int id;
    char nome[101];
} Categoria;

// Callback para a listagem de categorias
static int list_categories_callback(void *data, int argc, char **argv, char **azColName)
{
    // Imprime o cabeçalho uma única vez
    static int header_printed = 0;
    if (!header_printed)
    {
        printf("-------------------\n");
        printf("%-5s %-20s\n", "ID", "Nome");
        printf("-------------------\n");
        header_printed = 1;
    }

    printf("%-5s %-20s\n", argv[0], argv[1]);
    return 0;
}

void show_category_menu()
{
    int option = -1;

    while (option != 0)
    {
        printf("\n--- MENU DE CATEGORIAS ---\n");
        printf("1. Adicionar Categoria\n");
        printf("2. Listar Categorias\n");
        printf("3. Editar Categoria\n");
        printf("4. Deletar Categoria\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("--------------------------\n");

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            add_category();
            break;
        case 2:
            list_categories();
            break;
        case 3:
            edit_category();
            break;
        case 4:
            delete_category();
            break;
        case 0:
            printf("Voltando ao menu principal...\n");
            break;
        default:
            printf("Opção inválida!\n");
            break;
        }
    }
}

void add_category()
{
    char name[101];
    printf("\n--- ADICIONAR CATEGORIA ---\n");
    get_input("Nome da nova categoria: ", name, sizeof(name));

    if (strlen(name) == 0)
    {
        printf("O nome da categoria não pode ser vazio.\n");
        return;
    }

    char *sql = sqlite3_mprintf("INSERT INTO categorias (nome) VALUES ('%q');", name);
    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) == 0)
    {
        printf("Categoria '%s' adicionada com sucesso!\n", name);
    }
    else
    {
        printf("Erro ao adicionar categoria. Ela pode já existir.\n");
    }

    sqlite3_free(sql);
}

void list_categories()
{
    printf("\n--- LISTA DE CATEGORIAS ---\n");
    const char *sql = "SELECT id, nome FROM categorias ORDER BY nome;";

    // Reseta o flag do cabeçalho antes de chamar a query
    // (Gambiarra: um ponteiro para um int seria uma solução melhor)
    int (*callback)(void *, int, char **, char **) = list_categories_callback;
    // Acessando o ponteiro da função para resetar o estado
    // Esta é uma abordagem perigosa e não recomendada para produção.
    // Seria melhor passar um struct de contexto para o callback.
    // Por simplicidade, vamos apenas re-imprimir o cabeçalho sempre.
    // A solução com static int header_printed não funciona bem aqui.

    printf("%-5s %-20s\n", "ID", "Nome");
    printf("-------------------\n");
    execute_query(sql, callback, NULL);
    printf("-------------------\n");
}

void edit_category()
{
    int category_id;
    char new_name[101];

    printf("\n--- EDITAR CATEGORIA ---\n");
    list_categories(); // Mostra as categorias para o usuário saber qual ID editar
    
    printf("Digite o ID da categoria que deseja editar: ");
    char id_buffer[10];
    get_input("", id_buffer, sizeof(id_buffer));
    if (sscanf(id_buffer, "%d", &category_id) != 1)
    {
        printf("ID inválido.\n");
        return;
    }

    get_input("Digite o novo nome para a categoria: ", new_name, sizeof(new_name));

    if (strlen(new_name) == 0)
    {
        printf("O nome não pode ser vazio.\n");
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE categorias SET nome = '%q' WHERE id = %d;", new_name, category_id);
    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) == 0)
    {
        printf("Categoria atualizada com sucesso!\n");
    }
    else
    {
        printf("Erro ao atualizar categoria. Verifique se o ID existe ou se o novo nome já está em uso.\n");
    }

    sqlite3_free(sql);
}

void delete_category()
{
    int category_id;

    printf("\n--- DELETAR CATEGORIA ---\n");
    list_categories();
    
    printf("Digite o ID da categoria que deseja deletar: ");
    char id_buffer[10];
    get_input("", id_buffer, sizeof(id_buffer));
    if (sscanf(id_buffer, "%d", &category_id) != 1)
    {
        printf("ID inválido.\n");
        return;
    }

    char *sql = sqlite3_mprintf("DELETE FROM categorias WHERE id = %d;", category_id);
    if (!sql)
    {
        fprintf(stderr, "Erro ao alocar memória para o SQL.\n");
        return;
    }

    if (execute_non_query(sql) == 0)
    {
        printf("Categoria deletada com sucesso!\n");
    }
    else
    {
        printf("Erro ao deletar categoria. Verifique se o ID existe e se a categoria não está sendo usada por nenhum jogo.\n");
    }

    sqlite3_free(sql);
}
