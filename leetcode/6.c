#include <stdio.h>
#include <string.h>

/*for fucks sake i have no idea why this index is not within bounds, it might be a pointer issue idk*/
void push_back(char* str, int i, char c) {
    int len = strlen(str);
    if (i < 0 || i > len) {
        printf("Invalid i\n");
        return;
    }
    memmove(str + i + 1, str + i, len - i);
    str[i] = c;
}

char ziggy(char* s, int numRows) {
        char result;
        if (numRows == 1){
            return *s;
        }
        for (int row = 0; row < numRows; row++){
            int i = row;
            while (i < strlen(s)){
                push_back(&result, i, s[i]);
                i += (numRows - i % (numRows - 1) - 1) * 2;
            }
        }
    return result;
}

int main()
{
    char s[] = "PAYPALISHIRING";
    int rows = 4;
    ziggy(s, rows);
}

