/*
3. Longest Substring Without Repeating Characters

Given a string, find the length of the longest substring without repeating characters.

Examples:

Given "abcabcbb", the answer is "abc", which the length is 3.

Given "bbbbb", the answer is "b", with the length of 1.

Given "pwwkew", the answer is "wke", with the length of 3. Note that the answer must be a substring, "pwke" is a subsequence and not a substring.
*/
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
