#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

typedef struct darray {
    char**   item;
    uint16_t count;
    uint16_t capacity;
} darray;

typedef struct dict {
    darray     words;
    uint16_t*  indexTable;  // Flattened 2D array: row * colCount + col
    size_t     colCount;
    size_t     rowCount;
} dict;

dict* dictInit(const char* csv) {
    char *temp = strdup(csv);
    size_t rowCount = 0, colCount = 0;
    char *line = strtok(temp, "\n");
    if (line) {
        colCount = 1;
        for (char *c = line; *c; c++)
            if (*c == ',') colCount++;
        rowCount++;
        while ((line = strtok(NULL, "\n")) != NULL)
            rowCount++;
    }
    free(temp);

    dict *d = malloc(sizeof(dict));
    if (!d) return NULL;
    d->words.capacity = 16;
    d->words.count = 0;
    d->words.item = malloc(d->words.capacity * sizeof(char*));
    d->colCount = colCount;
    d->rowCount = rowCount;
    d->indexTable = malloc(rowCount * colCount * sizeof(uint16_t));

    char *csv_dup = strdup(csv);
    size_t current_row = 0;
    line = strtok(csv_dup, "\n");
    while(line != NULL) {
        size_t col = 0;
        char *token;
        char *saveptr;
        token = strtok_r(line, ",", &saveptr);
        while(token != NULL && col < colCount) {
            int found = -1;
            for (uint16_t i = 0; i < d->words.count; i++) {
                if (strcmp(d->words.item[i], token) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                if (d->words.count == d->words.capacity) {
                    d->words.capacity *= 2;
                    d->words.item = realloc(d->words.item, d->words.capacity * sizeof(char*));
                }
                d->words.item[d->words.count] = strdup(token);
                found = d->words.count;
                d->words.count++;
            }
            d->indexTable[current_row * colCount + col] = found;
            col++;
            token = strtok_r(NULL, ",", &saveptr);
        }
        current_row++;
        line = strtok(NULL, "\n");
    }
    free(csv_dup);
    return d;
}

char* dictQuery(dict* d, int row, int col) {
    if (!d || row < 0 || (size_t)row >= d->rowCount || col < 0 || (size_t)col >= d->colCount)
        return NULL;
    uint16_t idx = d->indexTable[row * d->colCount + col];
    return d->words.item[idx];
}

void dictPrint(const dict* d) {
    if (!d) return;
    for (size_t row = 0; row < d->rowCount; row++) {
        for (size_t col = 0; col < d->colCount; col++) {
            printf("%s", dictQuery((dict*)d, row, col));
            if (col < d->colCount - 1)
                printf(", ");
        }
        printf("\n");
    }
}

int main(void) {
    const char *csv_data =
    "dayOfTheWeek,wokeBeforeNoon,hasEaten,mealPlanned\n"
    "Sunday,no,no,Bread\n"
    "Sunday,no,no,Tofu\n"
    "Monday,no,no,Yams\n"
    "Tuesday,no,yes,Steak\n"
    "Thursday,yes,yes,Yams\n"
    "Saturday,yes,yes,Tofu\n"
    "Monday,yes,yes,Eggs\n"
    "Monday,yes,yes,Eggs\n"
    "Tuesday,no,yes,Bread\n"
    "Wednesday,no,yes,Banana\n"
    "Thursday,yes,yes,Eggs\n"
    "Friday,no,no,Yams\n"
    "Friday,yes,yes,Tofu\n"
    "Saturday,no,yes,Tofu\n"
    "Monday,no,no,Banana\n"
    "Friday,no,yes,Yams\n"
    "Saturday,yes,yes,Steak\n"
    "Tuesday,yes,yes,Eggs\n"
    "Sunday,yes,yes,Banana\n"
    "Saturday,yes,yes,Steak\n"
    "Thursday,no,no,Steak\n"
    "Friday,yes,no,Yams\n"
    "Monday,yes,no,Banana\n"
    "Tuesday,yes,yes,Waffle\n"
    "Sunday,no,no,Bread\n"
    "Wednesday,yes,no,Bread\n"
    "Thursday,no,yes,Banana\n"
    "Saturday,yes,no,Yams\n"
    "Tuesday,yes,no,Waffle\n"
    "Saturday,no,no,Eggs\n"
    "Saturday,yes,yes,Steak\n"
    "Sunday,yes,yes,Bread\n";

    dict *d = dictInit(csv_data);
    if (!d) {
        fprintf(stderr, "Failed to initialize dictionary\n");
        return 1;
    }

    size_t total_mem = sizeof(dict);
    total_mem += d->words.capacity * sizeof(char*);
    for (uint16_t i = 0; i < d->words.count; i++)
        total_mem += strlen(d->words.item[i]) + 1;
    total_mem += d->rowCount * d->colCount * sizeof(uint16_t);
    printf("Memory footprint of compressed CSV: %zu bytes\n", total_mem);

    clock_t start = clock();
    volatile char* result;
    for (int i = 0; i < 1000000; i++) {
        int row = i % d->rowCount;
        int col = i % d->colCount;
        result = dictQuery(d, row, col);
    }
    clock_t end = clock();
    double query_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken for querying: %f seconds\n", query_time);
    printf("Average time taken per querying: %f seconds\n", query_time / 1000000);

    dictPrint(d);

    for (uint16_t i = 0; i < d->words.count; i++)
        free(d->words.item[i]);
    free(d->words.item);
    free(d->indexTable);
    free(d);
    return 0;
}
