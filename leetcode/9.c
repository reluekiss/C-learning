#include <stdio.h>
#include <string.h>

char srev(char* s) {
    int len = strlen(s);
    for (int i = 0, j = len - 1; i <= j; i++, j--) {
        char c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
    return *s;
}

int main() {
    int x = -320;
    char c[7];
    
    sprintf(c,"%d", x);
    if(*c == srev(c)) {
        return 1;
    }
    return 0;
}
