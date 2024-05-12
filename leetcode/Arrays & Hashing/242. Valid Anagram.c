/*
* Given two strings s and t, return true if t is an anagram of s, and false otherwise.
*
* An Anagram is a word or phrase formed by rearranging the letters of a different word or phrase, typically using all the original letters exactly once.
* 
* Example 1:
* > Input: s = "anagram", t = "nagaram"
* > Output: true
* 
* Example 2:
* > Input: s = "rat", t = "car"
* > Output: false
* 
* Constraints:
* - 1 <= s.length, t.length <= 5 * 104
* - s and t consist of lowercase English letters.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int compareFunction(const void* a, const void* b) {    
    return (*(char*)a - *(char*)b);
}

bool isAnagram(const char* s, const char* t) {    
    size_t sLen = strlen(s);
    size_t tLen = strlen(t);

    if (sLen != tLen) {
        return false;
    }

    char sSorted[sLen + 1]; 
    char tSorted[tLen + 1]; 
    strcpy(sSorted, s);    
    strcpy(tSorted, t);

    qsort(sSorted, sLen, sizeof(char), compareFunction);
    qsort(tSorted, tLen, sizeof(char), compareFunction);

    if (memcmp(sSorted, tSorted, sLen) == 0) {
        return true;
    }

    return false;}

int main()
{
    const char s1[] = "anagram";
    const char t1[] = "nagaram";
    const char s2[] = "rat";
    const char t2[] = "cat";

    printf("Example 1: %s\n", isAnagram(s1, t1) ? "true" : "false");
    printf("Example 2: %s\n", isAnagram(s2, t2) ? "true" : "false");

    return 0;
}
