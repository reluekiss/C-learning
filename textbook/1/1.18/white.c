#include <stdio.h>
#include <stdlib.h>

// Returns pointer to last encountered printable char in string.
char *get_last_char(char *str) {
    char *index = str;
    char *ptr_last = NULL;
    for (; *index != '\0'; index++)
        if (*index >= ' ' && *index <= '~')
            ptr_last = index;
    return (ptr_last);
}

char* trim(char *str, size_t len) {
    char *ptr_last = get_last_char(str);
    if (!ptr_last)
        return (NULL);
    char *trimmed_line = calloc(sizeof(char), (ptr_last - str) + 1);
    if (!trimmed_line)
        return (NULL);
    for (int i = 0; &str[i] <= ptr_last; i++)
        trimmed_line[i] = str[i];
    return (trimmed_line);
}

int main () {
    char *buf = NULL;
    size_t bufsize = 80;
    while (getline(&buf, &bufsize, stdin)!= -1) {
        char *trimmed_line = trim(buf, bufsize);
        if (!trimmed_line) {
            free(buf);
            return 1;
        }
        printf("%s\n", trimmed_line);
        free(trimmed_line); 
    }

    if (buf!= NULL)
        free(buf);
    return (0);
} 
