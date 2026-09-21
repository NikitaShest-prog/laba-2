#include <stdio.h>
int main()
{
    int current_day = 1;
    int current_hour = 8;
    int inventory[10] = {0, 1, 3, 5, 5, 0, 2, 9, 1, 0};
    int choice;
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
                printf("Текущие время: День %d, 0%d:00\n", current_day, current_hour);
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
        }
    } while (choice!=0);
    printf("Конец игры");
}
