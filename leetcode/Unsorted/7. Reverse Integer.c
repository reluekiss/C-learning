/*
7. Reverse Integer

Reverse digits of an integer.


Example1: x =  123, return  321
Example2: x = -123, return -321


click to show spoilers.

Have you thought about this?

Here are some good questions to ask before coding. Bonus points for you if you have already thought through this!

If the integer's last digit is 0, what should the output be? ie, cases such as 10, 100.

Did you notice that the reversed integer might overflow? Assume the input is a 32-bit integer, then the reverse of 1000000003 overflows. How should you handle such cases?

For the purpose of this problem, assume that your function returns 0 when the reversed integer overflows.





Note:
The input is assumed to be a 32-bit signed integer. Your function should return 0 when the reversed integer overflows.
*/
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
