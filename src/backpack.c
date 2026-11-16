#include "backpack.h"

#include <stdio.h>
#include <string.h>

// Печатает все слоты, преобразуя ID предмета в его название.
void show_inventory(const int inventory[], const char item_names[][MAX_NAME_LEN]) {
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        printf("Слот %d: [%d] - %s\n", i, inventory[i], item_names[inventory[i]]);
    }
}

// Заменяет содержимое выбранного слота указанным предметом.
static void put_item(int inventory[], int slot, int item_id) {
    inventory[slot] = item_id;
}

// Запрашивает слот и ID предмета, проверяя допустимые диапазоны.
void menu_put_item(int inventory[]) {
    int slot = range_input("Введите индекс слота: ", 0, INVENTORY_SIZE - 1);
    int item_id = range_input("Введите ID предмета: ", 0, MAX_ITEMS - 1);
    put_item(inventory, slot, item_id);
}

// Очищает выбранный слот, записывая ID пустого предмета.
static void remove_item(int inventory[], int slot) {
    inventory[slot] = EMPTY_ITEM_ID;
}

// Запрашивает слот для очистки и проверяет его номер.
void menu_remove_item(int inventory[]) {
    int slot = range_input("Введите индекс слота: ", 0, INVENTORY_SIZE - 1);
    remove_item(inventory, slot);
}

// Ищет предмет с заданным ID в соседнем слоте слева или справа.
static int find_neighbor(const int inventory[], int slot, int item_id) {
    if (slot > 0 && inventory[slot - 1] == item_id) return slot - 1;
    if (slot < INVENTORY_SIZE - 1 && inventory[slot + 1] == item_id) return slot + 1;
    return NO_SLOT;
}

// Запрашивает два ID и сообщает, лежат ли предметы рядом.
void menu_find_neighbors(const int inventory[]) {
    int item1 = range_input("Введите ID первого предмета: ", 0, MAX_ITEMS - 1);
    int item2 = range_input("Введите ID второго предмета: ", 0, MAX_ITEMS - 1);
    bool found_first = false;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i] == item1) {
            found_first = true;
            int neighbor = find_neighbor(inventory, i, item2);
            if (neighbor != NO_SLOT) {
                printf("Слот первого предмета: %d; слот второго предмета: %d\n", i, neighbor);
            } else {
                printf("Не лежат рядом\n");
            }
        }
    }
    if (!found_first) printf("Первого предмета нет в инвентаре\n");
}

// Ищет ID предмета по точному названию в каталоге.
static int find_item_id(const char name[], const char item_names[][MAX_NAME_LEN]) {
    for (int id = 0; id < MAX_ITEMS; id++) {
        if (strcmp(name, item_names[id]) == 0) return id;
    }
    return ITEM_NOT_FOUND;
}

// Записывает номера слотов, в которых находится указанный предмет.
static int find_item_slots(const int inventory[], int item_id, int slots[]) {
    int count = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i] == item_id) slots[count++] = i;
    }
    return count;
}

// Запрашивает название предмета и выводит все его слоты.
void menu_search_item(const int inventory[], const char item_names[][MAX_NAME_LEN]) {
    char name[MAX_NAME_LEN];
    read_line("Введите название предмета: ", name, sizeof(name), MAX_NAME_LEN - 1);
    remove_newline(name);
    int item_id = find_item_id(name, item_names);
    if (item_id == ITEM_NOT_FOUND) {
        printf("Предмета \"%s\" нет в каталоге\n", name);
        return;
    }
    int slots[INVENTORY_SIZE];
    int count = find_item_slots(inventory, item_id, slots);
    if (count == 0) {
        printf("Предмета \"%s\" (ID %d) нет в рюкзаке\n", name, item_id);
    } else {
        printf("Предмет \"%s\" (ID %d) лежит в слотах:", name, item_id);
        for (int i = 0; i < count; i++) printf(" %d", slots[i]);
        printf("\n");
    }
}

// Дописывает в дневник имя фермера, время и содержимое инвентаря.
static bool write_diary(const char filename[], const char farmer_name[], int day, int hour,
                        const int inventory[], const char item_names[][MAX_NAME_LEN]) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) return false;
    fprintf(file, "===== Запись в дневнике =====\n");
    fprintf(file, "Фермер: %s\n", farmer_name);
    fprintf(file, "День %d, %02d:00\n", day, hour);
    fprintf(file, "Инвентарь:\n");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        fprintf(file, "  Слот %d: %s\n", i, item_names[inventory[i]]);
    }
    fprintf(file, "\n");
    fclose(file);
    return true;
}

// Вызывает сохранение дневника и сообщает об успехе или ошибке.
void menu_write_diary(const char farmer_name[], int day, int hour,
                      const int inventory[], const char item_names[][MAX_NAME_LEN]) {
    if (write_diary(DIARY_FILE, farmer_name, day, hour, inventory, item_names)) {
        printf("Запись добавлена в %s\n", DIARY_FILE);
    } else {
        printf("Ошибка: не удалось открыть %s для записи\n", DIARY_FILE);
    }
}

// Читает заказы, показывает доступные и записывает их статусы в файл.
void menu_bulletin_board(const int inventory[], const char item_names[][MAX_NAME_LEN], int current_day) {
    FILE *input = fopen(INPUT_FILE, "r");
    if (input == NULL) {
        printf("Ошибка: файл %s не найден в папке с программой\n", INPUT_FILE);
        return;
    }
    FILE *output = fopen(OUTPUT_FILE, "w");
    if (output == NULL) {
        printf("Ошибка: не удалось открыть %s для записи\n", OUTPUT_FILE);
        fclose(input);
        return;
    }

    char header[MAX_LINE_LEN];
    snprintf(header, sizeof(header), "===== Доска объявлений, день %d =====\n", current_day);
    fputs(header, stdout);
    fputs(header, output);

    char raw[MAX_LINE_LEN];
    char line[MAX_LINE_LEN];
    char customer[MAX_NAME_LEN];
    char item[MAX_NAME_LEN];
    int day, reward;
    int active = 0;
    while (fgets(raw, sizeof(raw), input) != NULL) {
        // Пропускает некорректные строки, не используя неинициализированные поля.
        if (sscanf(raw, " %d | %63[^|]| %63[^|]| %d", &day, customer, item, &reward) != 4) continue;
        remove_newline(customer);
        remove_newline(item);
        if (day > current_day) continue;

        bool ready = false;
        for (int i = 0; i < INVENTORY_SIZE; i++) {
            if (strcmp(item_names[inventory[i]], item) == 0) {
                ready = true;
                break;
            }
        }
        active++;
        snprintf(line, sizeof(line), "%d. Заказчик: %s | Нужно: %s | Награда: %d | %s\n",
                 active, customer, item, reward, ready ? READY_STATUS : IN_PROGRESS_STATUS);
        fputs(line, stdout);
        fputs(line, output);
    }
    if (active == 0) {
        fputs("Активных заданий нет\n", stdout);
        fputs("Активных заданий нет\n", output);
    }
    fclose(input);
    fclose(output);
    printf("Результат записан в %s\n", OUTPUT_FILE);
}
