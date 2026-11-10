#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <locale.h>   // setlocale — включает кодировку терминала (UTF-8), чтобы правильно считать русские буквы
#include <time.h> // time — получает время для начальной настройки генератора случайных чисел
#include <ctype.h> // toupper — позволяет принимать команды как маленькими, так и большими буквами

#define HOURS_IN_DAY 24       // часов в игровых сутках
#define START_DAY 1           // с какого дня начинается игра
#define START_HOUR 8          // с какого часа начинается игра
#define INVENTORY_SIZE 10     // количество слотов в рюкзаке
#define MAX_ITEMS 10          // количество видов предметов (ID от 0 до 9)
#define EMPTY_ITEM_ID 0       // ID "пустого" слота
#define MENU_ITEMS_COUNT 11   // количество пунктов меню (от 0 до 10)
#define MAX_WORK_HOURS 1000   // больше этого за раз работать нельзя (защита от огромных чисел)
#define NO_SLOT -1            // "слот не найден" — такого номера слота не бывает
#define MAX_FARMER_NAME_LEN 32 // максимальная длина имени фермера в символах (без учёта '\0')
#define MAX_BYTES_PER_CHAR 4   // в кодировке UTF-8 один символ занимает от 1 до 4 байт (латиница — 1, русская буква — 2)
#define MAX_NAME_LEN 64        // длина названия предмета в байтах вместе с '\0'
#define MAX_LINE_LEN 256       // максимальная длина строки, читаемой из файла
#define ITEMS_FILE "items.txt" // файл с названиями предметов
#define FILE_NOT_FOUND -1      // "файл не удалось открыть"
#define ITEM_NOT_FOUND -1      // "такого названия нет в каталоге" — такого ID не бывает
#define DIARY_FILE "diary.txt" // дневник фермера
#define INPUT_FILE "input.txt"     // заказы жителей деревни для доски объявлений
#define OUTPUT_FILE "output.txt"   // результат работы доски объявлений
#define READY_STATUS "[ГОТОВО К СДАЧЕ]"  // предмет есть в рюкзаке
#define IN_PROGRESS_STATUS "[В ПРОЦЕССЕ]" // предмета нет в рюкзаке

#define MIN_MAP_SIZE 5          // минимальная ширина и высота карты
#define MAX_MAP_SIZE 30         // максимальная ширина и высота карты
#define MIN_OBSTACLE_PERCENT 15 // минимальный процент препятствий при генерации
#define MAX_OBSTACLE_PERCENT 30 // максимальный процент препятствий при генерации
#define COLLAPSE_INTERVAL 5     // количество успешных перемещений между обрушениями
#define START_FLOOR 1           // номер первого этажа башни
#define WALL_CELL '#'          // символ непроходимой стены
#define FLOOR_CELL '.'         // символ свободного пола
#define TREE_CELL 'T'          // символ непроходимого дерева
#define ROCK_CELL 'R'          // символ непроходимого камня
#define HERO_CELL '@'          // символ героя при отображении карты
#define UP_STAIRS_CELL 'U'     // символ лестницы на следующий этаж
#define DOWN_STAIRS_CELL 'D'   // символ лестницы для возвращения домой
#define ABYSS_CELL ' '         // символ непроходимой бездны
#define PERCENT_SCALE 100      // количество процентов в целом, используем для проверки вероятности
#define OBSTACLE_TYPES 2       // количество видов препятствий: дерево и камень
#define BORDER_SIZE 1          // толщина внешней стены в клетках


int current_day = START_DAY;
int current_hour = START_HOUR;
void menu(const char farmer_name[]) {          
    printf("\n=== Ферма фермера %s ===\n", farmer_name);     // %s — печатает строку (массив char до '\0')
    printf("Меню:\n");
    printf("[0] выход из игры\n");
    printf("[1] посмотреть на часы\n");
    printf("[2] промотать время вперед\n");
    printf("[3] посмотреть инвентарь\n");
    printf("[4] положить предмет в слот\n");
    printf("[5] выбрость предмет\n");
    printf("[6] соедние ячейки\n");
    printf("[7] найти предмет в рюкзаке по названию\n");
    printf("[8] записать состояние в дневник\n");
    printf("[9] доска объявлений (вариант 9)\n");
    printf("[10] отправиться в приключение\n"); // показываем новый пункт меню
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

// Проверяет, состоит ли строка только из пробелов (или вообще пустая)
bool is_blank(const char str[]) {
    for (int i = 0; str[i] != '\0'; i++)      // идём по строке, пока не встретим нуль-терминатор '\0'
        if (str[i] != ' ' && str[i] != '\t')  // нашли "настоящий" символ — значит строка не пустая
            return false;
    return true;
}

// Считает количество символов (а не байт) в строке UTF-8.
// strlen("Никита") вернёт 12, потому что каждая русская буква — 2 байта, а эта функция вернёт 6.
int char_count(const char str[]) {
    size_t count = mbstowcs(NULL, str, 0);   // стандартная функция из stdlib.h: с NULL ничего не копирует, а только считает символы
    if (count == (size_t)-1)                 // -1 — строку не удалось разобрать (например, не настроена кодировка)
        return strlen(str);                  // тогда считаем по байтам — строже, но программа не сломается
    return count;
}

// Ввод строки целиком (вместе с пробелами) через fgets.
// size — размер массива str в байтах (вместе с местом под '\0'),
// max_chars — сколько символов максимум можно ввести.
// Переспрашивает, если строка пустая или слишком длинная.
void read_line(const char *prompt, char str[], int size, int max_chars) {
    bool flag = false;
    printf("%s", prompt);
    do {
        if (fgets(str, size, stdin) == NULL) {    // fgets возвращает NULL, если ввод закончился (EOF)
            printf("\nВвод закончился, выход из игры\n");
            exit(0);
        }
        int len = strlen(str);                    // strlen — сколько символов в строке до '\0'
        bool fits = true;                         // поместилась ли вся строка в массив
        if (len > 0 && str[len - 1] == '\n') {    // fgets сохраняет '\n' в конце строки —
            str[len - 1] = '\0';                  // заменяем его на '\0', чтобы он не попал в имя
        } else {                                  // '\n' нет — значит fgets прочитал не всю строку
            int c = getchar();                    // смотрим, что осталось во вводе
            if (c != '\n' && c != EOF) {          // там ещё символы — строка слишком длинная
                fits = false;
                clear_input_line();               // выбрасываем остаток, чтобы он не попал в следующий ввод
            }
        }
        if (!fits || char_count(str) > max_chars) {     // не влезло в массив или символов больше, чем можно
            printf("Слишком длинно, не больше %d символов: ", max_chars);
        } else if (is_blank(str)) {
            printf("Строка не может быть пустой: ");
        } else {
            flag = true;
        }
    } while (!flag);
}

void menu_work_through() {
    int time_work = range_input("Сколько времени вы хотите проработать: ", 0, MAX_WORK_HOURS);
    work_through(time_work);
}

// Убирает в конце строки '\n', который оставляет fgets, '\r' (бывает в файлах из Windows) и лишние пробелы —
// иначе "Дерево " с пробелом не совпадёт с "Дерево" при поиске через strcmp
void remove_newline(char str[]) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || str[len - 1] == ' ' || str[len - 1] == '\t')) {
        len--;
        str[len] = '\0';                     // ставим конец строки на место '\n'
    }
}

// Обработка: заполняет каталог item_names названиями из файла filename.
// Сначала кладёт стандартные названия, потом перезаписывает их тем, что прочитано из файла.
// Возвращает, сколько названий прочитано из файла, или FILE_NOT_FOUND, если файл не открылся.
int load_item_names(char item_names[][MAX_NAME_LEN], const char filename[]) {
    const char default_names[MAX_ITEMS][MAX_NAME_LEN] = {     // запасные названия, если файла нет
        "Пусто", "Дерево", "Камень", "Палка", "Грязь",
        "Яйцо", "Тухлое мясо", "Бутылка", "Кремень", "Уголь"
    };
    for (int i = 0; i < MAX_ITEMS; i++)
        strcpy(item_names[i], default_names[i]);              // strcpy — копирует строку (через "=" строки копировать нельзя)

    FILE *file = fopen(filename, "r");                         // "r" — открыть только для чтения
    if (file == NULL)                                          // NULL — файл не найден или нет доступа
        return FILE_NOT_FOUND;

    char line[MAX_LINE_LEN];
    int loaded = 0;                                            // сколько названий удалось прочитать
    while (fgets(line, sizeof(line), file) != NULL) {          // читаем файл по строке, пока он не закончится
        remove_newline(line);
        int id;
        int name_start = 0;                                    // с какого символа строки начинается название
        if (sscanf(line, "%d %n", &id, &name_start) != 1)      // %d — читаем ID, %n — запоминаем, сколько символов уже прочитано
            continue;                                          // в начале строки нет числа — пропускаем её
        const char *name = line + name_start;                  // название — это всё, что после числа и пробела
        if (id < 0 || id >= MAX_ITEMS || is_blank(name))       // ID вне 0..9 или нет названия — строка кривая, пропускаем
            continue;
        strncpy(item_names[id], name, MAX_NAME_LEN - 1);       // strncpy — копирует не больше MAX_NAME_LEN - 1 байт, чтобы не вылезти за массив
        item_names[id][MAX_NAME_LEN - 1] = '\0';               // strncpy не всегда ставит '\0', поэтому ставим его сами
        loaded++;
    }
    fclose(file);                                              // файл обязательно закрываем
    return loaded;
}

// [3] Вывод: печатает все слоты инвентаря с названиями предметов из каталога
void show_inventory(const int inventory[], const char item_names[][MAX_NAME_LEN]) {
    for (int i = 0; i < INVENTORY_SIZE; i++)
        printf("Слот %d: [%d] - %s\n", i, inventory[i], item_names[inventory[i]]);  // ID из слота -> название с этим ID
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

// [7] Обработка: ищет в каталоге ID предмета с названием name.
// Возвращает ID или ITEM_NOT_FOUND, если такого названия нет.
int find_item_id(const char name[], const char item_names[][MAX_NAME_LEN]) {
    for (int id = 0; id < MAX_ITEMS; id++)
        if (strcmp(name, item_names[id]) == 0)   // strcmp возвращает 0, если строки полностью совпадают ("==" для строк не работает)
            return id;
    return ITEM_NOT_FOUND;
}

// [7] Обработка: записывает в массив slots номера слотов, где лежит предмет item_id.
// Возвращает, сколько таких слотов нашлось.
int find_item_slots(const int inventory[], int item_id, int slots[]) {
    int count = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++)
        if (inventory[i] == item_id) {
            slots[count] = i;                   // запоминаем номер слота
            count++;
        }
    return count;
}

// [7] Ввод и вывод: спрашивает название, находит его ID и печатает слоты, где лежит предмет
void menu_search_item(const int inventory[], const char item_names[][MAX_NAME_LEN]) {
    char name[MAX_NAME_LEN];
    read_line("Введите название предмета: ", name, sizeof(name), MAX_NAME_LEN - 1);
    remove_newline(name);                                   // убираем случайные пробелы в конце, иначе strcmp не найдёт "Дерево "
    int item_id = find_item_id(name, item_names);
    if (item_id == ITEM_NOT_FOUND) {
        printf("Предмета \"%s\" нет в каталоге\n", name);  // \" — кавычка внутри строки
        return;                                             // дальше искать нечего — выходим из функции
    }
    int slots[INVENTORY_SIZE];                              // больше, чем слотов в рюкзаке, найти нельзя
    int count = find_item_slots(inventory, item_id, slots);
    if (count == 0) {
        printf("Предмета \"%s\" (ID %d) нет в рюкзаке\n", name, item_id);
    } else {
        printf("Предмет \"%s\" (ID %d) лежит в слотах:", name, item_id);
        for (int i = 0; i < count; i++)
            printf(" %d", slots[i]);
        printf("\n");
    }
}

// [8] Обработка: дописывает в конец файла filename сводку — имя, день, час и инвентарь словами.
// Возвращает true, если запись удалась, и false, если файл не открылся.
bool write_diary(const char filename[], const char farmer_name[], int day, int hour,
                 const int inventory[], const char item_names[][MAX_NAME_LEN]) {
    FILE *file = fopen(filename, "a");             // "a" — дозапись: старые записи остаются, новая добавляется в конец
    if (file == NULL)                              // например, нет прав на запись в папку
        return false;
    fprintf(file, "===== Запись в дневнике =====\n");                // fprintf — как printf, только пишет в файл
    fprintf(file, "Фермер: %s\n", farmer_name);
    fprintf(file, "День %d, %02d:00\n", day, hour);
    fprintf(file, "Инвентарь:\n");
    for (int i = 0; i < INVENTORY_SIZE; i++)
        fprintf(file, "  Слот %d: %s\n", i, item_names[inventory[i]]);   // предмет словами, по ID из каталога
    fprintf(file, "\n");                          // пустая строка между записями, чтобы их было легко различать
    fclose(file);                                  // без fclose данные могут не дописаться на диск
    return true;
}

// [8] Вывод: вызывает write_diary и сообщает пользователю результат
void menu_write_diary(const char farmer_name[], const int inventory[], const char item_names[][MAX_NAME_LEN]) {
    if (write_diary(DIARY_FILE, farmer_name, current_day, current_hour, inventory, item_names))
        printf("Запись добавлена в %s\n", DIARY_FILE);
    else
        printf("Ошибка: не удалось открыть %s для записи\n", DIARY_FILE);
}

// [9] Ввод и вывод с обработкой: читает INPUT_FILE, выводит на экран и записывает в OUTPUT_FILE
// только задания, доступные сегодня, со статусом по наличию предмета в рюкзаке
void menu_bulletin_board(const int inventory[], const char item_names[][MAX_NAME_LEN]) {
    FILE *input = fopen(INPUT_FILE, "r");                   // открываем файл с заказами для чтения
    if (input == NULL) {                                    // файла нет или нет доступа
        printf("Ошибка: файл %s не найден в папке с программой\n", INPUT_FILE);  // сообщаем пользователю
        return;                                             // выходим в меню
    }
    FILE *output = fopen(OUTPUT_FILE, "w");                 // открываем результат для записи (старый текст стирается)
    if (output == NULL) {                                   // не удалось создать файл
        printf("Ошибка: не удалось открыть %s для записи\n", OUTPUT_FILE);      // сообщаем пользователю
        fclose(input);                                      // закрываем уже открытый input
        return;                                             // выходим в меню
    }

    char header[MAX_LINE_LEN];                              // буфер под заголовок отчёта
    snprintf(header, sizeof(header), "===== Доска объявлений, день %d =====\n", current_day);  // собираем заголовок с текущим днём
    fputs(header, stdout);                                  // выводим заголовок на экран
    fputs(header, output);                                  // записываем заголовок в файл

    char raw[MAX_LINE_LEN];                                 // буфер для строки, прочитанной из файла
    char line[MAX_LINE_LEN];                                // буфер для строки отчёта
    char customer[MAX_NAME_LEN];                            // заказчик из текущей строки
    char item[MAX_NAME_LEN];                                // требуемый предмет из текущей строки
    int day, reward;                                        // день появления и награда из текущей строки
    int active = 0;                                         // сколько активных заданий уже выведено
    while (fgets(raw, sizeof(raw), input) != NULL) {        // читаем файл построчно, пока он не закончится
        // строка "День | Заказчик | Предмет | Награда"; пробелы вокруг | пропускаются
        sscanf(raw, " %d | %[^|]| %[^|]| %d", &day, customer, item, &reward);  // разбираем строку на четыре поля
        remove_newline(customer);                           // убираем пробелы и \n в конце заказчика
        remove_newline(item);                               // убираем пробелы и \n в конце предмета
        if (day > current_day)                              // задание ещё не появилось
            continue;                                       // пропускаем его и переходим к следующей строке
        bool ready = false;                                 // по умолчанию предмета в рюкзаке нет
        for (int i = 0; i < INVENTORY_SIZE; i++)            // проходим по всем слотам рюкзака
            if (strcmp(item_names[inventory[i]], item) == 0) {   // название предмета в слоте совпало с требуемым
                ready = true;                               // предмет есть в рюкзаке
                break;                                      // дальше искать не нужно
            }
        active++;                                           // увеличиваем счётчик активных заданий (он же номер)
        snprintf(line, sizeof(line), "%d. Заказчик: %s | Нужно: %s | Награда: %d | %s\n",   // собираем строку отчёта
                 active, customer, item, reward,            // номер, заказчик, предмет, награда
                 ready ? READY_STATUS : IN_PROGRESS_STATUS);    // статус: есть предмет или нет
        fputs(line, stdout);                                // выводим строку на экран
        fputs(line, output);                                // записываем строку в файл
    }
    if (active == 0) {                                      // ни одного активного задания не нашлось
        fputs("Активных заданий нет\n", stdout);            // сообщаем на экран
        fputs("Активных заданий нет\n", output);            // и записываем в файл
    }
    fclose(input);                                          // закрываем файл с заказами
    fclose(output);                                         // закрываем файл с результатом, данные сохраняются на диск
    printf("Результат записан в %s\n", OUTPUT_FILE);        // сообщаем, куда записан результат
}

void free_map(char **map, int height) { // освобождаем карту, зная адрес массива строк и их количество
    if (map == NULL) { // проверяем, была ли вообще выделена память под массив указателей
        return; // для отсутствующей карты освобождать нечего
    } // заканчиваем проверку отсутствующей карты
    for (int y = 0; y < height; y++) { // перебираем строки, память для которых была выделена
        free(map[y]); // освобождаем память текущей строки
    } // заканчиваем освобождение строк
    free(map); // после строк освобождаем сам массив указателей
} // заканчиваем функцию освобождения карты

char **create_map(int width, int height) { // создаём динамический двумерный массив и возвращаем его адрес
    if (width < MIN_MAP_SIZE || width > MAX_MAP_SIZE || height < MIN_MAP_SIZE || height > MAX_MAP_SIZE) { // проверяем допустимость размеров до выделения памяти
        return NULL; // сообщаем вызывающей функции о неправильных размерах
    } // заканчиваем проверку размеров
    char **map = malloc(height * sizeof(*map)); // выделяем память под height указателей на строки
    if (map == NULL) { // проверяем, удалось ли выделить массив указателей
        return NULL; // сообщаем об ошибке выделения памяти
    } // заканчиваем проверку массива указателей
    for (int y = 0; y < height; y++) { // выделяем память отдельно для каждой строки карты
        map[y] = malloc(width * sizeof(*map[y])); // выделяем width ячеек типа char для текущей строки
        if (map[y] == NULL) { // проверяем, удалось ли создать текущую строку
            free_map(map, y); // освобождаем только y уже созданных строк и массив указателей
            return NULL; // сообщаем об ошибке без утечки ранее выделенной памяти
        } // заканчиваем обработку ошибки выделения строки
    } // заканчиваем создание строк карты
    return map; // возвращаем созданную карту, ячейки которой заполним на следующем шаге
} // заканчиваем функцию создания карты

int random_between(int min, int max) { // возвращаем случайное целое число в диапазоне от min до max включительно
    return min + rand() % (max - min + 1); // сдвигаем случайное число в нужный диапазон
} // заканчиваем функцию случайного выбора

void generate_tower(char **map, int width, int height, int hero_x, int hero_y) { // заполняем уже выделенную карту башни
    int obstacle_percent = random_between(MIN_OBSTACLE_PERCENT, MAX_OBSTACLE_PERCENT); // выбираем вероятность препятствия для этажа
    int stairs_x; // сохраняем столбец лестницы вверх
    int stairs_y; // сохраняем строку лестницы вверх
    do { // выбираем лестницу отдельно от стартовой клетки героя
        stairs_x = random_between(BORDER_SIZE, width - BORDER_SIZE - 1); // выбираем столбец внутри стен
        stairs_y = random_between(BORDER_SIZE, height - BORDER_SIZE - 1); // выбираем строку внутри стен
    } while (stairs_x == hero_x && stairs_y == hero_y); // повторяем выбор, если лестница совпала со стартом
    int left = hero_x < stairs_x ? hero_x : stairs_x; // определяем левую границу горизонтальной части пути
    int right = hero_x > stairs_x ? hero_x : stairs_x; // определяем правую границу горизонтальной части пути
    int top = hero_y < stairs_y ? hero_y : stairs_y; // определяем верхнюю границу вертикальной части пути
    int bottom = hero_y > stairs_y ? hero_y : stairs_y; // определяем нижнюю границу вертикальной части пути
    for (int y = 0; y < height; y++) { // перебираем все строки карты
        for (int x = 0; x < width; x++) { // перебираем все клетки текущей строки
            bool border = x == 0 || y == 0 || x == width - 1 || y == height - 1; // определяем внешнюю границу карты
            bool path = (y == hero_y && x >= left && x <= right) || (x == stairs_x && y >= top && y <= bottom); // резервируем Г-образный путь от героя к лестнице
            if (border) { // внешние клетки должны стать стенами
                map[y][x] = WALL_CELL; // ставим непроходимую стену
            } else if (!path && random_between(1, PERCENT_SCALE) <= obstacle_percent) { // случайно размещаем препятствия вне гарантированного пути
                map[y][x] = random_between(1, OBSTACLE_TYPES) == 1 ? TREE_CELL : ROCK_CELL; // выбираем дерево или камень
            } else { // остальные внутренние клетки оставляем проходимыми
                map[y][x] = FLOOR_CELL; // записываем символ обычного пола
            } // заканчиваем выбор содержимого клетки
        } // заканчиваем заполнение строки
    } // заканчиваем заполнение всей карты
    map[hero_y][hero_x] = DOWN_STAIRS_CELL; // сохраняем лестницу вниз под начальной позицией героя
    map[stairs_y][stairs_x] = UP_STAIRS_CELL; // размещаем лестницу на следующий этаж
} // заканчиваем генерацию башни

void show_map(char **map, int width, int height, int hero_x, int hero_y) { // выводим карту, сохраняя содержимое клетки под героем
    for (int y = 0; y < height; y++) { // перебираем строки карты сверху вниз
        for (int x = 0; x < width; x++) { // перебираем клетки строки слева направо
            putchar(x == hero_x && y == hero_y ? HERO_CELL : map[y][x]); // на позиции героя рисуем @, остальные клетки берём из карты
        } // заканчиваем вывод клеток строки
        putchar('\n'); // переходим на следующую строку экрана
    } // заканчиваем вывод карты
} // заканчиваем функцию отображения

int adventure_command(void) { // читаем одну команду, не завершая программу до освобождения карты
    while (true) { // повторяем ввод до получения допустимой команды
        printf("W — вверх, A — влево, S — вниз, D — вправо, Q — домой: "); // показываем команды приключения
        int command = getchar(); // читаем первый символ введённой строки
        if (command == EOF) { // проверяем завершение потока ввода
            return EOF; // передаём конец ввода отдельно от команды Q
        } // заканчиваем проверку конца ввода
        bool clean = command == '\n' ? false : clear_input_line(); // отбрасываем остаток строки и проверяем отсутствие лишних символов
        command = toupper((unsigned char)command); // переводим команду в верхний регистр
        if (clean && (command == 'W' || command == 'A' || command == 'S' || command == 'D' || command == 'Q')) { // принимаем только одну допустимую команду
            return command; // передаём проверенную команду игровому циклу
        } // заканчиваем проверку команды
        printf("Введите одну английскую букву W, A, S, D или Q.\n"); // сообщаем об ошибочном вводе
    } // заканчиваем цикл проверки ввода
} // заканчиваем функцию чтения команды

bool move_hero(char **map, int width, int height, int *hero_x, int *hero_y, int command) { // изменяем координаты через указатели только при успешном движении
    int new_x = *hero_x; // начинаем расчёт с текущего столбца героя
    int new_y = *hero_y; // начинаем расчёт с текущей строки героя
    switch (command) { // определяем смещение по команде игрока
        case 'W': new_y--; break; // для движения вверх уменьшаем номер строки
        case 'A': new_x--; break; // для движения влево уменьшаем номер столбца
        case 'S': new_y++; break; // для движения вниз увеличиваем номер строки
        case 'D': new_x++; break; // для движения вправо увеличиваем номер столбца
        default: return false; // остальные команды не перемещают героя
    } // заканчиваем расчёт предполагаемой позиции
    if (new_x < 0 || new_x >= width || new_y < 0 || new_y >= height) { // проверяем границы до обращения к массиву
        return false; // запрещаем выход за границы карты
    } // заканчиваем проверку координат
    char cell = map[new_y][new_x]; // получаем содержимое клетки назначения
    if (cell != FLOOR_CELL && cell != UP_STAIRS_CELL && cell != DOWN_STAIRS_CELL) { // разрешаем ходить только по полу и лестницам
        return false; // запрещаем движение в стены, препятствия и бездну
    } // заканчиваем проверку проходимости
    *hero_x = new_x; // сохраняем новый столбец героя
    *hero_y = new_y; // сохраняем новую строку героя
    return true; // сообщаем об успешном перемещении
} // заканчиваем функцию движения

bool collapse_layer(char **map, int width, int height, int layer) { // обрушиваем один слой и сообщаем, исчезла ли лестница U
    int right = width - 1 - layer; // находим правую границу слоя
    int bottom = height - 1 - layer; // находим нижнюю границу слоя
    bool upstairs_lost = false; // запоминаем, была ли лестница U на этом слое
    for (int y = layer; y <= bottom; y++) { // перебираем строки оставшейся части этажа
        for (int x = layer; x <= right; x++) { // перебираем столбцы оставшейся части этажа
            if (x == layer || x == right || y == layer || y == bottom) { // выбираем только внешний слой оставшейся части
                if (map[y][x] == UP_STAIRS_CELL) { // проверяем лестницу до замены символа
                    upstairs_lost = true; // запоминаем, что лестница провалилась
                } // заканчиваем проверку лестницы
                map[y][x] = ABYSS_CELL; // превращаем клетку слоя в непроходимую бездну
            } // заканчиваем обработку клетки слоя
        } // заканчиваем перебор столбцов
    } // заканчиваем перебор строк
    return upstairs_lost; // передаём результат игровому циклу
} // заканчиваем функцию обрушения

bool play_floor(char **map, int width, int height, int hero_x, int hero_y) { // возвращаем true при подъёме, false при завершении приключения
    int steps = 0; // начинаем отсчёт успешных шагов на новом этаже
    int layer = BORDER_SIZE; // первым обрушится слой сразу за внешней стеной
    bool emergency_exit = false; // до потери лестницы U действует обычное правило выхода
    while (true) { // обрабатываем команды на текущем этаже
        show_map(map, width, height, hero_x, hero_y); // показываем карту с героем поверх его текущей клетки
        int command = adventure_command(); // получаем проверенную команду игрока
        if (command == EOF) { // обрабатываем конец ввода в любой клетке
            return false; // завершаем приключение, чтобы вызывающая функция освободила карту
        } // заканчиваем проверку конца ввода
        if (command == 'Q') { // игрок хочет вернуться на ферму
            if (emergency_exit || map[hero_y][hero_x] == DOWN_STAIRS_CELL) { // разрешаем выход при эвакуации или на лестнице D
                return false; // завершаем приключение и передаём карту на освобождение
            } // заканчиваем проверку лестницы вниз
            printf("Чтобы вернуться домой, встаньте на лестницу D.\n"); // объясняем, где доступен выход
            continue; // остаёмся на этаже и ждём следующую команду
        } // заканчиваем обработку Q
        if (!move_hero(map, width, height, &hero_x, &hero_y, command)) { // пытаемся изменить координаты героя
            printf("Туда пройти нельзя!\n"); // сообщаем о стене, препятствии или границе карты
            continue; // после неудачного движения ждём следующую команду
        } // заканчиваем обработку результата движения
        steps++; // засчитываем только успешное перемещение
        if (steps == COLLAPSE_INTERVAL) { // после пяти шагов обрушиваем очередной слой
            steps = 0; // начинаем новый отсчёт до следующего обрушения
            bool upstairs_lost = collapse_layer(map, width, height, layer); // превращаем слой в бездну и проверяем лестницу U
            layer++; // в следующий раз обрушим слой ближе к центру
            printf("Внешний слой пола обрушился!\n"); // сообщаем об изменении карты
            if (map[hero_y][hero_x] == ABYSS_CELL) { // проверяем, обрушилась ли клетка под героем
                printf("Пол под героем обрушился. Эвакуация на ферму!\n"); // объясняем причину завершения приключения
                return false; // возвращаемся на ферму через освобождение карты
            } // заканчиваем проверку позиции героя
            if (upstairs_lost) { // проверяем, исчезла ли лестница вверх при этом обрушении
                emergency_exit = true; // разрешаем выход с любой оставшейся клетки до конца этажа
                printf("Лестница U провалилась. Нажмите Q для срочной эвакуации.\n"); // показываем команду срочного выхода
            } // заканчиваем обработку потери лестницы
        } // заканчиваем обработку пятого шага
        if (map[hero_y][hero_x] == UP_STAIRS_CELL) { // проверяем, наступил ли герой на лестницу вверх
            return true; // просим вызывающую функцию создать следующий этаж
        } // заканчиваем проверку лестницы вверх
    } // заканчиваем цикл этажа
} // заканчиваем функцию игры на одном этаже

void menu_adventure(void) { // управляем созданием и освобождением этажей башни
    int floor_number = START_FLOOR; // начинаем каждое приключение с первого этажа
    while (true) { // создаём по одному этажу за проход цикла
        int width = random_between(MIN_MAP_SIZE, MAX_MAP_SIZE); // выбираем ширину нового этажа
        int height = random_between(MIN_MAP_SIZE, MAX_MAP_SIZE); // выбираем высоту нового этажа
        char **map = create_map(width, height); // выделяем память под новый этаж
        if (map == NULL) { // проверяем, удалось ли выделить память
            printf("Не удалось выделить память для карты.\n"); // сообщаем об ошибке создания этажа
            return; // возвращаемся в меню фермы
        } // заканчиваем проверку выделения памяти
        int hero_x = random_between(BORDER_SIZE, width - BORDER_SIZE - 1); // выбираем стартовый столбец героя
        int hero_y = random_between(BORDER_SIZE, height - BORDER_SIZE - 1); // выбираем стартовую строку героя
        generate_tower(map, width, height, hero_x, hero_y); // заполняем карту и ставим D под героем
        printf("Создан этаж %d, размер карты %dx%d.\n", floor_number, width, height); // показываем номер и размеры этажа
        bool go_up = play_floor(map, width, height, hero_x, hero_y); // узнаём, поднялся ли герой или закончил приключение
        free_map(map, height); // освобождаем текущую карту до перехода или выхода
        if (!go_up) { // проверяем, закончилось ли приключение
            return; // возвращаемся в меню фермы после освобождения карты
        } // заканчиваем проверку выхода
        floor_number++; // увеличиваем номер этажа перед генерацией следующей карты
    } // заканчиваем цикл генерации этажей
} // заканчиваем функцию и возвращаемся в главное меню

int main()
{
    int permissible_numbers[MENU_ITEMS_COUNT] = {0,1,2,3,4,5,6,7,8,9,10}; // разрешаем выбор всех пунктов от 0 до 10
    int inventory[INVENTORY_SIZE] = {0, 1, 3, 5, 5, 0, 2, 9, 1, 0};
    int choice = 0;
    srand((unsigned int)time(NULL)); // один раз на запуск задаём начальное состояние генератора случайных чисел
    setlocale(LC_ALL, "");   // "" — взять кодировку из настроек системы, без этого mbstowcs не понимает русские буквы
    char item_names[MAX_ITEMS][MAX_NAME_LEN];                    // каталог: item_names[ID] — название предмета с этим ID
    int loaded = load_item_names(item_names, ITEMS_FILE);
    if (loaded == FILE_NOT_FOUND) {                              // сообщения пишем здесь, а не в load_item_names (там только обработка)
        printf("Внимание: файл %s не найден, используются стандартные названия предметов\n", ITEMS_FILE);
    } else if (loaded < MAX_ITEMS) {
        printf("Внимание: из %s прочитано %d из %d названий, остальные — стандартные\n", ITEMS_FILE, loaded, MAX_ITEMS);
    }
    char farmer_name[MAX_FARMER_NAME_LEN * MAX_BYTES_PER_CHAR + 1];
    read_line("Как зовут вашего фермера? ", farmer_name, sizeof(farmer_name), MAX_FARMER_NAME_LEN);
    printf("Добро пожаловать на ферму, %s!\n", farmer_name);
    do {
        menu(farmer_name);
        choice = number_input("Выберите пункт меню: ", permissible_numbers, MENU_ITEMS_COUNT);
        switch (choice) {
            case 1:
                current_time(current_day,current_hour);
                break;
            case 2:
                menu_work_through();
                break;
            case 3:
                show_inventory(inventory, item_names);
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
            case 7:
                menu_search_item(inventory, item_names);
                break;
            case 8:
                menu_write_diary(farmer_name, inventory, item_names);
                break;
            case 9:
                menu_bulletin_board(inventory, item_names);
                break;
            case 10: // обрабатываем выбор приключения
                menu_adventure(); // запускаем приключение в башне
                break; // завершаем обработку пункта и возвращаемся к циклу меню
        }
    } while (choice!=0);
    printf("Конец игры");
}
