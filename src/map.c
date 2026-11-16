#include "map.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "utils.h"

// Возвращает случайное целое число в указанном диапазоне включительно.
static int random_between(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Освобождает строки карты и массив указателей на них.
static void free_map(char **map, int height) {
    if (map == NULL) return;
    for (int y = 0; y < height; y++) free(map[y]);
    free(map);
}

// Выделяет динамический двумерный массив символов заданного размера.
static char **create_map(int width, int height) {
    if (width < MIN_MAP_SIZE || width > MAX_MAP_SIZE ||
        height < MIN_MAP_SIZE || height > MAX_MAP_SIZE) return NULL;

    char **map = malloc((size_t)height * sizeof(*map));
    if (map == NULL) return NULL;
    for (int y = 0; y < height; y++) {
        map[y] = malloc((size_t)width * sizeof(*map[y]));
        if (map[y] == NULL) {
            free_map(map, y);
            return NULL;
        }
    }
    return map;
}

// Заполняет этаж стенами, проходом, препятствиями и лестницами.
static void generate_tower(char **map, int width, int height, int hero_x, int hero_y) {
    int obstacle_percent = random_between(MIN_OBSTACLE_PERCENT, MAX_OBSTACLE_PERCENT);
    int stairs_x;
    int stairs_y;
    do {
        stairs_x = random_between(BORDER_SIZE, width - BORDER_SIZE - 1);
        stairs_y = random_between(BORDER_SIZE, height - BORDER_SIZE - 1);
    } while (stairs_x == hero_x && stairs_y == hero_y);

    int left = hero_x < stairs_x ? hero_x : stairs_x;
    int right = hero_x > stairs_x ? hero_x : stairs_x;
    int top = hero_y < stairs_y ? hero_y : stairs_y;
    int bottom = hero_y > stairs_y ? hero_y : stairs_y;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool border = x == 0 || y == 0 || x == width - 1 || y == height - 1;
            bool path = (y == hero_y && x >= left && x <= right) ||
                        (x == stairs_x && y >= top && y <= bottom);
            if (border) {
                map[y][x] = WALL_CELL;
            } else if (!path && random_between(1, PERCENT_SCALE) <= obstacle_percent) {
                map[y][x] = random_between(1, OBSTACLE_TYPES) == 1 ? TREE_CELL : ROCK_CELL;
            } else {
                map[y][x] = FLOOR_CELL;
            }
        }
    }
    map[hero_y][hero_x] = DOWN_STAIRS_CELL;
    map[stairs_y][stairs_x] = UP_STAIRS_CELL;
}

// Показывает карту, рисуя героя поверх клетки под ним.
static void show_map(char **map, int width, int height, int hero_x, int hero_y) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            putchar(x == hero_x && y == hero_y ? HERO_CELL : map[y][x]);
        }
        putchar('\n');
    }
}

// Читает одну команду приключения и проверяет, что введён ровно один символ.
static int adventure_command(void) {
    while (true) {
        printf("W — вверх, A — влево, S — вниз, D — вправо, Q — домой: ");
        int command = getchar();
        if (command == EOF) return EOF;
        bool clean = command == '\n' ? false : clear_input_line();
        command = toupper((unsigned char)command);
        if (clean && (command == 'W' || command == 'A' || command == 'S' ||
                      command == 'D' || command == 'Q')) return command;
        printf("Введите одну английскую букву W, A, S, D или Q.\n");
    }
}

// Передвигает героя, только если целевая клетка доступна.
static bool move_hero(char **map, int width, int height, int *hero_x, int *hero_y, int command) {
    int new_x = *hero_x;
    int new_y = *hero_y;
    switch (command) {
        case 'W': new_y--; break;
        case 'A': new_x--; break;
        case 'S': new_y++; break;
        case 'D': new_x++; break;
        default: return false;
    }
    if (new_x < 0 || new_x >= width || new_y < 0 || new_y >= height) return false;
    char cell = map[new_y][new_x];
    if (cell != FLOOR_CELL && cell != UP_STAIRS_CELL && cell != DOWN_STAIRS_CELL) return false;
    *hero_x = new_x;
    *hero_y = new_y;
    return true;
}

// Обрушивает внешний слой этажа и сообщает, исчезла ли лестница вверх.
static bool collapse_layer(char **map, int width, int height, int layer) {
    int right = width - 1 - layer;
    int bottom = height - 1 - layer;
    bool upstairs_lost = false;
    for (int y = layer; y <= bottom; y++) {
        for (int x = layer; x <= right; x++) {
            if (x == layer || x == right || y == layer || y == bottom) {
                if (map[y][x] == UP_STAIRS_CELL) upstairs_lost = true;
                map[y][x] = ABYSS_CELL;
            }
        }
    }
    return upstairs_lost;
}

// Обрабатывает команды на одном этаже; true означает, что герой поднялся выше.
static bool play_floor(char **map, int width, int height, int hero_x, int hero_y) {
    int steps = 0;
    int layer = BORDER_SIZE;
    bool emergency_exit = false;
    while (true) {
        show_map(map, width, height, hero_x, hero_y);
        int command = adventure_command();
        if (command == EOF) return false;
        if (command == 'Q') {
            if (emergency_exit || map[hero_y][hero_x] == DOWN_STAIRS_CELL) return false;
            printf("Чтобы вернуться домой, встаньте на лестницу D.\n");
            continue;
        }
        if (!move_hero(map, width, height, &hero_x, &hero_y, command)) {
            printf("Туда пройти нельзя!\n");
            continue;
        }
        steps++;
        if (steps == COLLAPSE_INTERVAL) {
            steps = 0;
            bool upstairs_lost = collapse_layer(map, width, height, layer);
            layer++;
            printf("Внешний слой пола обрушился!\n");
            if (map[hero_y][hero_x] == ABYSS_CELL) {
                printf("Пол под героем обрушился. Эвакуация на ферму!\n");
                return false;
            }
            if (upstairs_lost) {
                emergency_exit = true;
                printf("Лестница U провалилась. Нажмите Q для срочной эвакуации.\n");
            }
        }
        if (map[hero_y][hero_x] == UP_STAIRS_CELL) return true;
    }
}

// Создаёт этажи по очереди и освобождает память после каждого этажа.
void menu_adventure(void) {
    int floor_number = START_FLOOR;
    while (true) {
        int width = random_between(MIN_MAP_SIZE, MAX_MAP_SIZE);
        int height = random_between(MIN_MAP_SIZE, MAX_MAP_SIZE);
        char **map = create_map(width, height);
        if (map == NULL) {
            printf("Не удалось выделить память для карты.\n");
            return;
        }
        int hero_x = random_between(BORDER_SIZE, width - BORDER_SIZE - 1);
        int hero_y = random_between(BORDER_SIZE, height - BORDER_SIZE - 1);
        generate_tower(map, width, height, hero_x, hero_y);
        printf("Создан этаж %d, размер карты %dx%d.\n", floor_number, width, height);
        bool go_up = play_floor(map, width, height, hero_x, hero_y);
        free_map(map, height);
        if (!go_up) return;
        floor_number++;
    }
}
