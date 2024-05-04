#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char srev(char* s) {
    int len = strlen(s);
    if (s[0] = '-') {
        s[0] = '0';
    }
    for (int i = 0, j = len - 1; i <= j; i++, j--) {
        char c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
    memmove(void *, const void *, unsigned long) 
    return *s;
}

int main() {
    int x = -320;
    char c[7];
    
    sprintf(c,"%d", x);
    srev(c);
    x = atoi(c);

    printf("%d\n", x);
    return 0;
}
