#include "player.h"

#include <stdio.h>

#include "utils.h"

// Переводит игровое время вперёд и учитывает переход через полночь.
void work_through(int *current_day, int *current_hour, int hours) {
    int total_hours = *current_hour + hours;
    *current_day += total_hours / HOURS_IN_DAY;
    *current_hour = total_hours % HOURS_IN_DAY;
}

// Показывает текущий игровой день и час.
void current_time(int day, int hour) {
    printf("Текущее время: День %d, %02d:00\n", day, hour);
}

// Запрашивает длительность работы и сдвигает игровое время.
void menu_work_through(int *current_day, int *current_hour) {
    int hours = range_input("Сколько времени вы хотите проработать: ", 0, MAX_WORK_HOURS);
    work_through(current_day, current_hour, hours);
}
