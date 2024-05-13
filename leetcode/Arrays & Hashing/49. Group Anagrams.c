/*
* Given an array of strings strs, group the anagrams together. You can return the answer in any order.
* 
* An Anagram is a word or phrase formed by rearranging the letters of a different word or phrase, typically using all the original letters exactly once.
* 
* Example 1:
* 
* Input: strs = ["eat","tea","tan","ate","nat","bat"]
* Output: [["bat"],["nat","tan"],["ate","eat","tea"]]
*
* Example 2:
* 
* Input: strs = [""]
* Output: [[""]]
*
* Example 3:
* 
* Input: strs = ["a"]
* Output: [["a"]]
* 
* Constraints:
* 
* 1 <= strs.length <= 104
* 0 <= strs[i].length <= 100
* strs[i] consists of lowercase English letters.
*
* Not complete, it seg faults, no idea why.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRS 10004
#define MAX_STR_LEN 101

typedef struct {
    char **anagrams;
    int size;
    int capacity;
} AnagramGroup;

AnagramGroup *createAnagramGroup(int initialCapacity) {
    AnagramGroup *group = (AnagramGroup *)malloc(sizeof(AnagramGroup));
    group->anagrams = (char **)malloc(initialCapacity * sizeof(char *));
    group->size = 0;
    group->capacity = initialCapacity;
    return group;
}

void addAnagram(AnagramGroup *group, char *str) {
    if (group->size == group->capacity) {
        group->capacity *= 2;
        group->anagrams = (char **)realloc(group->anagrams, group->capacity * sizeof(char *));
    }
    group->anagrams[group->size++] = str;
}

int compare(const void *a, const void *b) {
    return strcmp(*(char **)a, *(char **)b);
}

char ***groupAnagrams(char **strs, int strsSize, int *returnSize, int **returnColumnSizes) {
    AnagramGroup *groups[MAX_STRS] = {NULL};
    int groupCount = 0;
    
    for (int i = 0; i < strsSize; i++) {
        char sortedStr[MAX_STR_LEN];
        strcpy(sortedStr, strs[i]);
        qsort(sortedStr, strlen(sortedStr), sizeof(char), compare);
    
        int found = 0;
        for (int j = 0; j < groupCount; j++) {
            if (strcmp(groups[j]->anagrams[0], sortedStr) == 0) {
                addAnagram(groups[j], strs[i]);
                found = 1;
                break;
            }
        }

    if (!found) {
        AnagramGroup *newGroup = createAnagramGroup(1);
        addAnagram(newGroup, strs[i]);
        groups[groupCount++] = newGroup;
    }
}

char ***result = (char ***)malloc((groupCount + 1) * sizeof(char **));
*returnColumnSizes = (int *)malloc((groupCount + 1) * sizeof(int));
*returnSize = groupCount;

for (int i = 0; i < groupCount; i++) {
    result[i] = groups[i]->anagrams;
    (*returnColumnSizes)[i] = groups[i]->size;
}
result[groupCount] = NULL;

for (int i = 0; i < groupCount; i++) {
    free(groups[i]->anagrams);
    free(groups[i]);
}

return result;}

int main() {
    char *strs[] = {"eat", "tea", "tan", "ate", "nat", "bat"};
    int strsSize = sizeof(strs) / sizeof(strs[0]);
    int returnSize, *returnColumnSizes;
    char ***result = groupAnagrams(strs, strsSize, &returnSize, &returnColumnSizes);
    
    for (int i = 0; i < returnSize; i++) {
        for (int j = 0; j < returnColumnSizes[i]; j++) {
            printf("%s ", result[i][j]);
        }
        printf("\n");
    }
    
    for (int i = 0; i < returnSize; i++) {
        free(result[i]);
    }

    free(result);
    free(returnColumnSizes);

    return 0;
}
