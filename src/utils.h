#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Общие константы проекта, используемые несколькими модулями.
#define HOURS_IN_DAY 24
#define START_DAY 1
#define START_HOUR 8
#define INVENTORY_SIZE 10
#define MAX_ITEMS 10
#define EMPTY_ITEM_ID 0
#define MENU_ITEMS_COUNT 11
#define MAX_WORK_HOURS 1000
#define NO_SLOT -1
#define MAX_FARMER_NAME_LEN 32
#define MAX_BYTES_PER_CHAR 4
#define MAX_NAME_LEN 64
#define MAX_LINE_LEN 256
#define ITEMS_FILE "items.txt"
#define FILE_NOT_FOUND -1
#define ITEM_NOT_FOUND -1
#define DIARY_FILE "diary.txt"
#define INPUT_FILE "input.txt"
#define OUTPUT_FILE "output.txt"
#define READY_STATUS "[ГОТОВО К СДАЧЕ]"
#define IN_PROGRESS_STATUS "[В ПРОЦЕССЕ]"
#define PERCENT_SCALE 100

// Проверяет ввод, читает строки и загружает каталог предметов.
bool is_in_list(int value, const int allowed[], int count);
bool clear_input_line(void);
int number_input(const char *prompt, const int allowed[], int count);
int range_input(const char *prompt, int min, int max);
bool is_blank(const char str[]);
int char_count(const char str[]);
void read_line(const char *prompt, char str[], int size, int max_chars);
void remove_newline(char str[]);
int load_item_names(char item_names[][MAX_NAME_LEN], const char filename[]);

#endif
