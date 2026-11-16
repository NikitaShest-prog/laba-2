#ifndef PLAYER_H
#define PLAYER_H

// Функции игрового времени, пока работающие с отдельными значениями дня и часа.
void work_through(int *current_day, int *current_hour, int hours);
void current_time(int day, int hour);
void menu_work_through(int *current_day, int *current_hour);

#endif
