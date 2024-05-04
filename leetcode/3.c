#include <stdio.h>
int lengthOfLongestSubstring(char * s)
{
    int max = 1;
    int count = 1;
    int start = 0;
    
    if (s[0] == '\0')
        return 0;
    if (s[1] == '\0')
        return 1;

    for (int i = 1; s[i] != '\0'; i++) {
        for (int j = start; j < i; j++) {
            if (s[i] != s[j]) {
                count++;
            } else {
                start = j + 1;
                break;
            }
        }
        if (max < count)
            max = count;
        count = 1;
    }  
    return max;
}

int main()
{
    char s[] = "pwwkew";
    printf("%d", lengthOfLongestSubstring(s));
    return 0;
}
