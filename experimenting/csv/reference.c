#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char **header;      // Array of header strings
    size_t numCols;
    char ***rows;       // 2D array: rows[row][col]
    size_t numRows;
    char **dict;        // Dictionary for unique strings
    size_t dictSize;
    size_t dictCap;
} CompressedCSV;

static inline char *intern(CompressedCSV *csv, const char *s) {
    for (size_t i = 0; i < csv->dictSize; i++) {
        if (!strcmp(csv->dict[i], s))
            return csv->dict[i];
    }
    if (csv->dictSize == csv->dictCap) {
        csv->dictCap = csv->dictCap ? csv->dictCap * 2 : 4;
        csv->dict = realloc(csv->dict, csv->dictCap * sizeof(char *));
    }
    csv->dict[csv->dictSize] = strdup(s);
    return csv->dict[csv->dictSize++];
}

CompressedCSV *compressCSV(const char *csvText) {
    CompressedCSV *csv = malloc(sizeof(CompressedCSV));
    csv->dict = NULL; csv->dictSize = 0; csv->dictCap = 0;
    csv->header = NULL; csv->rows = NULL;
    
    char *data = strdup(csvText);
    size_t lineCount = 0;
    char **lines = NULL;
    for (char *line = strtok(data, "\n"); line; line = strtok(NULL, "\n")) {
        lines = realloc(lines, (lineCount + 1) * sizeof(char *));
        lines[lineCount++] = line;
    }
    
    char *hdr = lines[0];
    size_t cols = 1;
    for (char *p = hdr; *p; p++) {
        if (*p == ',') cols++;
    }
    csv->numCols = cols;
    csv->header = malloc(cols * sizeof(char *));
    size_t col = 0;
    for (char *tok = strtok(hdr, ","); tok; tok = strtok(NULL, ","))
        csv->header[col++] = intern(csv, tok);
    
    csv->numRows = lineCount - 1;
    csv->rows = malloc(csv->numRows * sizeof(char **));
    for (size_t i = 1; i < lineCount; i++) {
        csv->rows[i - 1] = malloc(cols * sizeof(char *));
        col = 0;
        for (char *tok = strtok(lines[i], ","); tok && col < cols; tok = strtok(NULL, ","))
            csv->rows[i - 1][col++] = intern(csv, tok);
    }
    free(lines);
    free(data);
    return csv;
}

const char *getCSVValue(const CompressedCSV *csv, size_t row, size_t col) {
    if (row >= csv->numRows || col >= csv->numCols)
        return NULL;
    return csv->rows[row][col];
}

char *decompressCSV(const CompressedCSV *csv) {
    size_t totalLength = 0;
    for (size_t i = 0; i < csv->numCols; i++) {
        totalLength += strlen(csv->header[i]);
        if (i < csv->numCols - 1)
            totalLength++; // comma
    }
    totalLength++; // newline

    for (size_t i = 0; i < csv->numRows; i++) {
        for (size_t j = 0; j < csv->numCols; j++) {
            totalLength += strlen(csv->rows[i][j]);
            if (j < csv->numCols - 1)
                totalLength++; // comma
        }
        totalLength++; // newline
    }

    char *result = malloc(totalLength + 1);
    if (!result) return NULL;
    char *p = result;

    for (size_t i = 0; i < csv->numCols; i++) {
        size_t len = strlen(csv->header[i]);
        memcpy(p, csv->header[i], len);
        p += len;
        if (i < csv->numCols - 1)
            *p++ = ',';
    }
    *p++ = '\n';

    for (size_t i = 0; i < csv->numRows; i++) {
        for (size_t j = 0; j < csv->numCols; j++) {
            size_t len = strlen(csv->rows[i][j]);
            memcpy(p, csv->rows[i][j], len);
            p += len;
            if (j < csv->numCols - 1)
                *p++ = ',';
        }
        *p++ = '\n';
    }
    *p = '\0';
    return result;
}

void freeCompressedCSV(CompressedCSV *csv) {
    if (!csv) return;
    for (size_t i = 0; i < csv->dictSize; i++)
        free(csv->dict[i]);
    free(csv->dict);
    for (size_t i = 0; i < csv->numRows; i++)
        free(csv->rows[i]);
    free(csv->rows);
    free(csv->header);
    free(csv);
}

static double timespec_to_sec(const struct timespec *ts) {
    return ts->tv_sec + ts->tv_nsec / 1e9;
}

int main(void) {
    const char *csvData =
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

    struct timespec start, end;
    double compressTime, accessTime, decompressTime;

    clock_gettime(CLOCK_MONOTONIC, &start);
    CompressedCSV *csv = compressCSV(csvData);
    clock_gettime(CLOCK_MONOTONIC, &end);
    compressTime = timespec_to_sec(&end) - timespec_to_sec(&start);
    if (!csv) {
        fprintf(stderr, "Compression failed\n");
        return EXIT_FAILURE;
    }

    volatile size_t dummy = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t iter = 0; iter < 1000000; iter++) {
        for (size_t r = 0; r < csv->numRows; r++) {
            for (size_t c = 0; c < csv->numCols; c++) {
                const char *val = getCSVValue(csv, r, c);
                dummy += strlen(val);
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    accessTime = timespec_to_sec(&end) - timespec_to_sec(&start);

    clock_gettime(CLOCK_MONOTONIC, &start);
    char *decompressed = decompressCSV(csv);
    clock_gettime(CLOCK_MONOTONIC, &end);
    decompressTime = timespec_to_sec(&end) - timespec_to_sec(&start);
    if (!decompressed) {
        fprintf(stderr, "Decompression failed\n");
        freeCompressedCSV(csv);
        return EXIT_FAILURE;
    }

    size_t overheadMemUsage = sizeof(CompressedCSV);
    size_t dataMemUsage = 0;
    dataMemUsage += csv->numCols * sizeof(char*); // header array
    dataMemUsage += csv->numRows * sizeof(char**); // rows array
    dataMemUsage += csv->numRows * csv->numCols * sizeof(char*); // each row's pointer array
    for (size_t i = 0; i < csv->dictSize; i++) {
        dataMemUsage += strlen(csv->dict[i]) + 1;
    }

    printf("Non-mmaped CSV Profiling Results:\n");
    printf("Compression time:   %.6f seconds\n", compressTime);
    printf("Access time:        %.6f seconds (dummy sum: %zu)\n", accessTime, dummy);
    printf("Decompression time: %.6f seconds\n", decompressTime);
    printf("Overhead memory usage: %zu bytes\n", overheadMemUsage);
    printf("Dict memory usage: %zu bytes\n", dataMemUsage);

    printf("\nDecompressed CSV:\n%s", decompressed);

    free(decompressed);
    freeCompressedCSV(csv);
    return EXIT_SUCCESS;
}
