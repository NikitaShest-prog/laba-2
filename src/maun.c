#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <locale.h>   // setlocale — включает кодировку терминала (UTF-8), чтобы правильно считать русские буквы

#define HOURS_IN_DAY 24       // часов в игровых сутках
#define START_DAY 1           // с какого дня начинается игра
#define START_HOUR 8          // с какого часа начинается игра
#define INVENTORY_SIZE 10     // количество слотов в рюкзаке
#define MAX_ITEMS 10          // количество видов предметов (ID от 0 до 9)
#define EMPTY_ITEM_ID 0       // ID "пустого" слота
#define MENU_ITEMS_COUNT 10   // количество пунктов меню (от 0 до 9)
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

int main()
{
    int permissible_numbers[MENU_ITEMS_COUNT] = {0,1,2,3,4,5,6,7,8,9};
    int inventory[INVENTORY_SIZE] = {0, 1, 3, 5, 5, 0, 2, 9, 1, 0};
    int choice = 0;
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
        }
    } while (choice!=0);
    printf("Конец игры");
}
