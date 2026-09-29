#include <stdio.h>
int main()
{
    int buffer_time = 0;
    int current_day = 1;
    int current_hour = 8;
    int time_work = 0;
    int inventory[10] = {0, 1, 3, 5, 5, 0, 2, 9, 1, 0};
    int choice = 0;
    int slot_index = 0;
    int ID_item1 = 0;
    int ID_item2 = 0;
    int size_inventory = sizeof(inventory) / sizeof(inventory[0]);
    do {
        printf("Меню:\n");
        printf("[0] выход из игры\n");
        printf("[1] посмотреть на часы\n");
        printf("[2] промотать время вперед\n");
        printf("[3] посмотреть инвентарь\n");
        printf("[4] положить предмет в слот\n");
        printf("[5] выбрость предмет\n");
        printf("[6] соедние ячейки\n");
        printf("Выберите пункт меню: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                if (current_hour < 10) {
                    printf("Текущие время: День %d, 0%d:00\n", current_day, current_hour);
                } else {
                    printf("Текущие время: День %d, %d:00\n", current_day, current_hour);
                }
                    break;
            case 2:
                printf("Сколько часов вы хотите проработать: ");
                if (scanf("%d", &time_work)==1) {
                    buffer_time = current_hour;
                    buffer_time += time_work;
                    current_day += buffer_time/24;
                    current_hour += buffer_time%24;
                } else {
                    printf("Вы глупец\n");
                    scanf("%*s");
                }
                break;
            case 3:
                for (int i = 0; i < size_inventory; i++) {
                    printf("Слот %d: [%d]\n", i, inventory[i]);
                }
                break;
            case 4:
                printf("Введите индекс слота от 0 до 9: ");
                if (scanf("%d", &slot_index) == 1) {
                    printf("Введите ID предмета: ");
                    if (scanf("%d", &ID_item1) == 1) {
                        if (slot_index <= 9 && slot_index >= 0) {
                            inventory[slot_index] = ID_item1;
                        } else {
                            printf("Вы глупец\n");
                        }
                    } else {
                        printf("Вы глупец\n");
                        scanf("%*s");
                    }
                } else {
                    printf("Вы глупец\n");
                    scanf("%*s");
                }
                break;
            case 5:
                printf("Введите индекс слота от 0 до 9: ");
                    if (scanf("%d", &slot_index) == 1) {
                    if (slot_index <= 9 & slot_index >= 0) {
                        inventory[slot_index] = 0;
                    } else {
                        printf("Вы глупец\n");
                    }
                } else {
                    printf("Вы глупец\n");
                    scanf("%*s");
                }
                break;
            case 6:
                printf("Введите ID первого предмета: ");
                if (scanf("%d", &ID_item1) == 1) {
                    printf("Введите ID второго предмета: ");
                    if (scanf("%d", &ID_item2) == 1) {
                        scanf("%d", &ID_item2);
                        for (int i = 0; i < size_inventory; i++) {
                            if (inventory[i] == ID_item1) {
                                switch (i) {
                                case 9:
                                    if (inventory[i-1] == ID_item2) {
                                        printf("ID первого предмета: %d ID второго предмета: %d\n", i, i-1);
                                    } else {
                                        printf("Не лежат рядом\n");
                                    }
                                    break;
                                case 0:
                                    if (inventory[i+1] == ID_item2) {
                                        printf("ID первого предмета: %d ID второго предмета: %d\n", i, i+1);
                                    } else {
                                        printf("Не лежат рядом\n");
                                    }
                                    break;
                                default:
                                    if (inventory[i-1] == ID_item2 || inventory[i+1] == ID_item2) {
                                        if (inventory[i-1] == ID_item2) {
                                            printf("ID первого предмета: %d ID второго предмета: %d\n", i, i-1);
                                        } else {
                                            printf("ID первого предмета: %d ID второго предмета: %d\n", i, i+1);
                                        } 
                                    } else {
                                        printf("Не лежат рядом\n");
                                    }
                                }
                            }
                        }
                    } else {
                        printf("Вы глупец\n");
                        scanf("%*s");
                    }
                } else {
                    printf("Вы глупец\n");
                    scanf("%*s");
                }
                break;
        }
    } while (choice!=0);
    printf("Конец игры");
}
