#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "backpack.h"
#include "map.h"
#include "player.h"
#include "utils.h"

// Печатает главное меню фермера.
static void menu(const char farmer_name[]) {
    printf("\n=== Ферма фермера %s ===\n", farmer_name);
    printf("Меню:\n");
    printf("[0] выход из игры\n");
    printf("[1] посмотреть на часы\n");
    printf("[2] промотать время вперёд\n");
    printf("[3] посмотреть инвентарь\n");
    printf("[4] положить предмет в слот\n");
    printf("[5] выбросить предмет\n");
    printf("[6] проверить соседние ячейки\n");
    printf("[7] найти предмет в рюкзаке по названию\n");
    printf("[8] записать состояние в дневник\n");
    printf("[9] доска объявлений (старый вариант 9)\n");
    printf("[10] отправиться в приключение\n");
}

// Запускает игру и связывает меню с функциями отдельных модулей.
int main(void) {
    const int permissible_numbers[MENU_ITEMS_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int inventory[INVENTORY_SIZE] = {0, 1, 3, 5, 5, 0, 2, 9, 1, 0};
    int current_day = START_DAY;
    int current_hour = START_HOUR;
    int choice;

    // Инициализирует локаль и генератор случайных чисел один раз при запуске.
    srand((unsigned int)time(NULL));
    setlocale(LC_ALL, "");

    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    int loaded = load_item_names(item_names, ITEMS_FILE);
    if (loaded == FILE_NOT_FOUND) {
        printf("Внимание: файл %s не найден, используются стандартные названия предметов\n", ITEMS_FILE);
    } else if (loaded < MAX_ITEMS) {
        printf("Внимание: из %s прочитано %d из %d названий, остальные — стандартные\n",
               ITEMS_FILE, loaded, MAX_ITEMS);
    }

    char farmer_name[MAX_FARMER_NAME_LEN * MAX_BYTES_PER_CHAR + 1];
    read_line("Как зовут вашего фермера? ", farmer_name, sizeof(farmer_name), MAX_FARMER_NAME_LEN);
    printf("Добро пожаловать на ферму, %s!\n", farmer_name);

    // Повторяет меню, пока пользователь не выберет выход.
    do {
        menu(farmer_name);
        choice = number_input("Выберите пункт меню: ", permissible_numbers, MENU_ITEMS_COUNT);
        switch (choice) {
            case 1:
                current_time(current_day, current_hour);
                break;
            case 2:
                menu_work_through(&current_day, &current_hour);
                break;
            case 3:
                show_inventory(inventory, item_names);
                break;
            case 4:
                menu_put_item(inventory);
                break;
            case 5:
                menu_remove_item(inventory);
                break;
            case 6:
                menu_find_neighbors(inventory);
                break;
            case 7:
                menu_search_item(inventory, item_names);
                break;
            case 8:
                menu_write_diary(farmer_name, current_day, current_hour, inventory, item_names);
                break;
            case 9:
                menu_bulletin_board(inventory, item_names, current_day);
                break;
            case 10:
                menu_adventure();
                break;
            default:
                break;
        }
    } while (choice != 0);

    printf("Конец игры\n");
    return 0;
}
