#include <stdio.h>
#include <stdbool.h>
int current_day = 1;
int current_hour = 8;
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
    int buffer_time = 0;
    buffer_time = current_hour;
    buffer_time += time_work;
    current_day += buffer_time/24;
    current_hour += buffer_time%24;
}
void current_time(int day, int hour) {
     if (hour < 10) {
        printf("Текущие время: День %d, 0%d:00\n", day, hour);
    } else {
        printf("Текущие время: День %d, %d:00\n", day, hour);
    }
}
bool is_in_list(int value, const int allowed[], int count) {
    for(int i = 0; i < count; i++)
        if (allowed[i] == value)
            return true;
    return false;
}
int number_input(const char *prompt, const int allowed[], int count) { //при передачи allowed == NULL и count == 0 работает с любыми числами
    int num;
    bool flag = false;
    printf("%s", prompt);
    do {
        if (scanf("%d", &num)!=1) {
            printf("Введите коректное число: ");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        } else if (allowed != NULL && !is_in_list(num, allowed, count)) {
            printf("Введите число из доступных значений: ");
        } else {
            flag = true;
        }
    } while (!flag);
    return num;
}
int main()
{
    int permissible_numbers[7] = {0,1,2,3,4,5,6};
    int time_work = 0;
    int inventory[10] = {0, 1, 3, 5, 5, 0, 2, 9, 1, 0};
    int choice = 0;
    int slot_index = 0;
    int ID_item1 = 0;
    int ID_item2 = 0;
    int size_inventory = sizeof(inventory) / sizeof(inventory[0]);
    do {
        menu();
        choice = number_input("Выберите пункт меню: ", permissible_numbers, 7);
        switch (choice) {
            case 1:
                current_time(current_day,current_hour);
                break;
            case 2:
                time_work = number_input("Сколько времени вы хотите проработать: ", NULL, 0);
                work_through(time_work);
                // printf("Сколько часов вы хотите проработать: ");
                // if (scanf("%d", &time_work)==1) {
                //     buffer_time = current_hour;
                //     buffer_time += time_work;
                //     current_day += buffer_time/24;
                //     current_hour += buffer_time%24;
                // } else {
                //     printf("Вы глупец\n");
                //     scanf("%*s");
                // }
                break;
            case 3:
                for (int i = 0; i < size_inventory; i++) {
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
                        printf("Слот %d: [%d] [Грязь]]\n", i, inventory[i]);
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
                        for (int i = 0; i < size_inventory; i++) {
                            if (inventory[i] == ID_item1) {
                                switch (i) {
                                case 9:
                                    if (inventory[i-1] == ID_item2) {
                                        printf("Слот первого предмета: %d Слот второго предмета: %d\n", i, i-1);
                                    } else {
                                        printf("Не лежат рядом\n");
                                    }
                                    break;
                                case 0:
                                    if (inventory[i+1] == ID_item2) {
                                        printf("Слот первого предмета: %d Слот второго предмета: %d\n", i, i+1);
                                    } else {
                                        printf("Не лежат рядом\n");
                                    }
                                    break;
                                default:
                                    if (inventory[i-1] == ID_item2 || inventory[i+1] == ID_item2) {
                                        if (inventory[i-1] == ID_item2) {
                                            printf("Слот первого предмета: %d Слот второго предмета: %d\n", i, i-1);
                                        } else if (inventory[i+1] == ID_item2) {
                                            printf("Слот первого предмета: %d Слот второго предмета: %d\n", i, i+1);
                                        } 
                                    } else {
                                        printf("Не лежат рядом\n");
                                    }
                                    break;
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
