#include <stdio.h>
int main()
{
    int buffer_time;
    int current_day = 1;
    int current_hour = 8;
    int time_work;
    const int inventory[10] = {0, 1, 3, 5, 5, 0, 2, 9, 1, 0};
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
