#include <stdio.h>
#include <string.h>

void printCharNTimes(char c, int n) {
    char arr[n + 1];
    memset(arr, c, n);
    arr[n] = '\0';
    printf("%s\n", arr);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    int c, i;
    char h[] = "#";
    int k[128] = {0}; 
    char *input = argv[1];
    int len = strlen(input);
    for (i = 0; i < len; ++i) {
        c = input[i];
        k[c]++;
    }
    printf("characters: frequency\n");
    for (i = 0; i < 128; ++i) {
        if (k[i] > 0) {
            printf("%c: %d ", i, k[i]);
            printCharNTimes(*h, k[i]);
        }
    }
    return 0;
}
