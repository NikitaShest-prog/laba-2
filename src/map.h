#ifndef MAP_H
#define MAP_H

// Константы генерации и отображения карты приключения.
#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30
#define MIN_OBSTACLE_PERCENT 15
#define MAX_OBSTACLE_PERCENT 30
#define COLLAPSE_INTERVAL 5
#define START_FLOOR 1
#define WALL_CELL '#'
#define FLOOR_CELL '.'
#define TREE_CELL 'T'
#define ROCK_CELL 'R'
#define HERO_CELL '@'
#define UP_STAIRS_CELL 'U'
#define DOWN_STAIRS_CELL 'D'
#define ABYSS_CELL ' '
#define OBSTACLE_TYPES 2
#define BORDER_SIZE 1

// Запускает приключение с генерацией и освобождением этажей карты.
void menu_adventure(void);

#endif
