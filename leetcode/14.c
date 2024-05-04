#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *lcp(char **strs, int strsSize) {
   for(int c=0;; ++c)
   {
      if(strs[0][c] == '\0')
         return strs[0];
      for(int s=1; s<strsSize; ++s)
      {
         if(strs[s][c] != strs[0][c])
         {
            strs[0][c] = '\0';
            return strs[0];
         }
      }
   }
}

int main()
{
    int numStrings = 3;
    int maxLength = 6;
    char** strs = (char**)malloc(numStrings * sizeof(char*));
    for(int i = 0; i < numStrings; i++) {
        strs[i] = (char*)malloc(maxLength * sizeof(char));
        strcpy(strs[i], "example");
    }
    printf("%s", lcp(strs, numStrings));
    for(int i = 0; i < numStrings; i++) {
        free(strs[i]);
    }
    free(strs);
    return 0;
}
