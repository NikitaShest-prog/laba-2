#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

// Проверяет, входит ли число в список разрешённых значений.
bool is_in_list(int value, const int allowed[], int count) {
    for (int i = 0; i < count; i++) {
        if (allowed[i] == value) return true;
    }
    return false;
}

// Считывает остаток строки и сообщает, были ли в нём лишние символы.
bool clear_input_line(void) {
    int c;
    bool clean = true;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (c != ' ' && c != '\t') clean = false;
    }
    return clean;
}

// Запрашивает целое число, повторяя запрос при ошибочном вводе.
int number_input(const char *prompt, const int allowed[], int count) {
    int num;
    bool valid = false;
    printf("%s", prompt);
    do {
        int read_count = scanf("%d", &num);
        if (read_count == EOF) {
            printf("\nВвод закончился, выход из игры\n");
            exit(0);
        }
        bool clean = clear_input_line();
        if (read_count != 1 || !clean) {
            printf("Введите корректное число: ");
        } else if (allowed != NULL && !is_in_list(num, allowed, count)) {
            printf("Введите число из доступных значений: ");
        } else {
            valid = true;
        }
    } while (!valid);
    return num;
}

// Запрашивает целое число в заданном диапазоне.
int range_input(const char *prompt, int min, int max) {
    int num = number_input(prompt, NULL, 0);
    while (num < min || num > max) {
        printf("Число должно быть от %d до %d: ", min, max);
        num = number_input("", NULL, 0);
    }
    return num;
}

// Проверяет, что строка пуста или состоит только из пробелов и табуляций.
bool is_blank(const char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ' && str[i] != '\t') return false;
    }
    return true;
}

// Считает символы UTF-8, а если строка некорректна — считает байты.
int char_count(const char str[]) {
    size_t count = mbstowcs(NULL, str, 0);
    if (count == (size_t)-1) return (int)strlen(str);
    return (int)count;
}

// Считывает непустую строку целиком, включая пробелы, с проверкой длины.
void read_line(const char *prompt, char str[], int size, int max_chars) {
    bool valid = false;
    printf("%s", prompt);
    do {
        if (fgets(str, size, stdin) == NULL) {
            printf("\nВвод закончился, выход из игры\n");
            exit(0);
        }
        int len = (int)strlen(str);
        bool fits = true;
        if (len > 0 && str[len - 1] == '\n') {
            str[len - 1] = '\0';
        } else {
            int c = getchar();
            if (c != '\n' && c != EOF) {
                fits = false;
                clear_input_line();
            }
        }
        if (!fits || char_count(str) > max_chars) {
            printf("Слишком длинно, не больше %d символов: ", max_chars);
        } else if (is_blank(str)) {
            printf("Строка не может быть пустой: ");
        } else {
            valid = true;
        }
    } while (!valid);
}

// Удаляет перевод строки и пробелы в конце строки.
void remove_newline(char str[]) {
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' ||
                       str[len - 1] == ' ' || str[len - 1] == '\t')) {
        str[--len] = '\0';
    }
}

// Загружает названия предметов и оставляет стандартные названия для отсутствующих строк.
int load_item_names(char item_names[][MAX_NAME_LEN], const char filename[]) {
    const char default_names[MAX_ITEMS][MAX_NAME_LEN] = {
        "Пусто", "Дерево", "Камень", "Палка", "Грязь",
        "Яйцо", "Тухлое мясо", "Бутылка", "Кремень", "Уголь"
    };
    for (int i = 0; i < MAX_ITEMS; i++) strcpy(item_names[i], default_names[i]);

    FILE *file = fopen(filename, "r");
    if (file == NULL) return FILE_NOT_FOUND;

    char line[MAX_LINE_LEN];
    int loaded = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        remove_newline(line);
        int id;
        int name_start = 0;
        if (sscanf(line, "%d %n", &id, &name_start) != 1) continue;
        const char *name = line + name_start;
        if (id < 0 || id >= MAX_ITEMS || is_blank(name)) continue;
        strncpy(item_names[id], name, MAX_NAME_LEN - 1);
        item_names[id][MAX_NAME_LEN - 1] = '\0';
        loaded++;
    }
    fclose(file);
    return loaded;
}
