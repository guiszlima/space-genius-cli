#include "StoreController.h"
#include "../../Utils/Input.h"
#include "../Game/GameController.h"            
#include "../Cart/CartController.h"            
#include "../../Wishlist/WishlistController.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void show_store_menu()
{
    int option = -1;

    while (option != 0)
    {
        printf("\n--- LOJA DE JOGOS ---\n");
        extern void list_games(); 
        list_games();
        printf("-----------------------\n");
        printf("1. Adicionar Jogo ao Carrinho\n");
        printf("2. Adicionar Jogo à Lista de Desejos\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("-----------------------\n");

        option = get_menu_option("Escolha uma opção: ");

        switch (option)
        {
        case 1:
            add_to_cart(); 
            break;
        case 2:
            add_to_wishlist(); 
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
