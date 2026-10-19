#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define HOURS_IN_DAY 24       // часов в игровых сутках
#define START_DAY 1           // с какого дня начинается игра
#define START_HOUR 8          // с какого часа начинается игра
#define INVENTORY_SIZE 10     // количество слотов в рюкзаке
#define MAX_ITEMS 10          // количество видов предметов (ID от 0 до 9)
#define EMPTY_ITEM_ID 0       // ID "пустого" слота
#define MENU_ITEMS_COUNT 7    // количество пунктов меню (от 0 до 6)
#define MAX_WORK_HOURS 1000   // больше этого за раз работать нельзя (защита от огромных чисел)
#define NO_SLOT -1            // "слот не найден" — такого номера слота не бывает

int current_day = START_DAY;
int current_hour = START_HOUR;
void menu() {
    printf("Меню:\n");
    printf("[0] выход из игры\n");
    printf("[1] посмотреть на часы\n");
    printf("[2] промотать время вперед\n");
    printf("[3] посмотреть инвентарь\n");
    printf("[4] положить предмет в слот\n");
    printf("[5] выбрость предмет\n");
    printf("[6] соедние ячейки\n");
}

void work_through(int time_work) {
    int total_hours = current_hour + time_work;
    current_day += total_hours / HOURS_IN_DAY;
    current_hour = total_hours % HOURS_IN_DAY;
}
void current_time(int day, int hour) {
    printf("Текущие время: День %d, %02d:00\n", day, hour);
}
bool is_in_list(int value, const int allowed[], int count) {
    for(int i = 0; i < count; i++)
        if (allowed[i] == value)
            return true;
    return false;
}
bool clear_input_line() {
    int c;
    bool clean = true;
    while ((c = getchar()) != '\n' && c != EOF)
        if (c != ' ' && c != '\t')
            clean = false;
    return clean;
}
int number_input(const char *prompt, const int allowed[], int count) {
    int num;
    bool flag = false;
    printf("%s", prompt);
    do {
        int read_count = scanf("%d", &num);
        if (read_count == EOF) {
            printf("\nВвод закончился, выход из игры\n");
            exit(0);
        }
        bool clean = clear_input_line();
        if (read_count != 1 || !clean) {
            printf("Введите коректное число: ");
        } else if (allowed != NULL && !is_in_list(num, allowed, count)) {
            printf("Введите число из доступных значений: ");
        } else {
            flag = true;
        }
    } while (!flag);
    return num;
}

int range_input(const char *prompt, int min, int max) {
    int num = number_input(prompt, NULL, 0);
    while (num < min || num > max) {
        printf("Число должно быть от %d до %d: ", min, max);
        num = number_input("", NULL, 0);
    }
    return num;
}

void menu_work_through() {
    int time_work = range_input("Сколько времени вы хотите проработать: ", 0, MAX_WORK_HOURS);
    work_through(time_work);
}

void show_inventory(const int inventory[]) {
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        switch (inventory[i])
        {
        case 0:
            printf("Слот %d: [%d]\n", i, inventory[i]);
            break;
        case 1:
            printf("Слот %d: [%d] [Дерево]\n", i, inventory[i]);
            break;
        case 2:
            printf("Слот %d: [%d] [Камень]\n", i, inventory[i]);
            break;
        case 3:
            printf("Слот %d: [%d] [Палка]\n", i, inventory[i]);
            break;
        case 4:
            printf("Слот %d: [%d] [Грязь]\n", i, inventory[i]);
            break;
        case 5:
            printf("Слот %d: [%d] [Яйцо]\n", i, inventory[i]);
            break;
        case 6:
            printf("Слот %d: [%d] [Тухлое мясо]\n", i, inventory[i]);
            break;
        case 7:
            printf("Слот %d: [%d] [Бутылка]\n", i, inventory[i]);
            break;
        case 8:
            printf("Слот %d: [%d] [Кремень]\n", i, inventory[i]);
            break;
        case 9:
            printf("Слот %d: [%d] [Уголь]\n", i, inventory[i]);
            break;
        default:
            break;
        }
    }
}

void put_item(int inventory[], int slot, int item_id) {
    inventory[slot] = item_id;
}

void menu_put_item(int inventory[]) {
    int slot = range_input("Введите индекс слота: ", 0, INVENTORY_SIZE - 1);
    int item_id = range_input("Введите ID предмета: ", 0, MAX_ITEMS - 1);
    put_item(inventory, slot, item_id);
}

void remove_item(int inventory[], int slot) {
    inventory[slot] = EMPTY_ITEM_ID;
}

void menu_remove_item(int inventory[]) {
    int slot = range_input("Введите индекс слота: ", 0, INVENTORY_SIZE - 1);
    remove_item(inventory, slot);
}

int find_neighbor(const int inventory[], int slot, int item_id) {
    if (slot > 0 && inventory[slot - 1] == item_id)
        return slot - 1;
    if (slot < INVENTORY_SIZE - 1 && inventory[slot + 1] == item_id)
        return slot + 1;
    return NO_SLOT;
}

void menu_find_neighbors(const int inventory[]) {
    int item1 = range_input("Введите ID первого предмета: ", 0, MAX_ITEMS - 1);
    int item2 = range_input("Введите ID второго предмета: ", 0, MAX_ITEMS - 1);
    bool found_first = false;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i] == item1) {
            found_first = true;
            int neighbor = find_neighbor(inventory, i, item2);
            if (neighbor != NO_SLOT) {
                printf("Слот первого предмета: %d Слот второго предмета: %d\n", i, neighbor);
            } else {
                printf("Не лежат рядом\n");
            }
        }
    }
    if (!found_first) {
        printf("Первого предмета нет в инвентаре\n");
    }
}

int main()
{
    int permissible_numbers[MENU_ITEMS_COUNT] = {0,1,2,3,4,5,6};
    int inventory[INVENTORY_SIZE] = {0, 1, 3, 5, 5, 0, 2, 9, 1, 0};
    int choice = 0;
    do {
        menu();
        choice = number_input("Выберите пункт меню: ", permissible_numbers, MENU_ITEMS_COUNT);
        switch (choice) {
            case 1:
                current_time(current_day,current_hour);
                break;
            case 2:
                menu_work_through();
                break;
            case 3:
                show_inventory(inventory);
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
        }
    } while (choice!=0);
    printf("Конец игры");
}
