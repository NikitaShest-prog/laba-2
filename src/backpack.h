#ifndef BACKPACK_H
#define BACKPACK_H

#include "utils.h"

// Операции со слотами рюкзака и отображение предметов.
void show_inventory(const int inventory[], const char item_names[][MAX_NAME_LEN]);
void menu_put_item(int inventory[]);
void menu_remove_item(int inventory[]);
void menu_find_neighbors(const int inventory[]);
void menu_search_item(const int inventory[], const char item_names[][MAX_NAME_LEN]);

// Запись состояния инвентаря в дневник и показ доски объявлений.
void menu_write_diary(const char farmer_name[], int day, int hour,
                      const int inventory[], const char item_names[][MAX_NAME_LEN]);
void menu_bulletin_board(const int inventory[], const char item_names[][MAX_NAME_LEN], int current_day);

#endif
