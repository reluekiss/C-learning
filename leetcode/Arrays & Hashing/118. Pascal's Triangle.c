/*
* Given an integer numRows, return the first numRows of Pascal's triangle.
* 
* In Pascal's triangle, each number is the sum of the two numbers directly above it as shown:
* 
* Example 1:
* 
* Input: numRows = 5
* Output: [[1],[1,1],[1,2,1],[1,3,3,1],[1,4,6,4,1]]
* Example 2:
* 
* Input: numRows = 1
* Output: [[1]]
*  
* Constraints:
* 
* 1 <= numRows <= 30
*/

#include <stdlib.h>
#include <stdio.h>

int** generate(int numRows, int* returnSize, int** returnColumnSizes) {
    *returnSize = numRows;
    (*returnColumnSizes) = malloc(sizeof(int*)*numRows);
    int** ans = malloc(sizeof(int*)*numRows);

    // Handle initial cases
    for (int i=0; i < numRows; i++) {
        (*returnColumnSizes)[i] = i+1;
        ans[i] = malloc(sizeof(int)*(i+1));
        ans[i][0] = 1;
        ans[i][i] = 1;
    }

    // Enumerate and adjust pointers to generate triangle
    for (int i=2; i < numRows; i++) {
        for (int j=1; j < i; j++) {
            ans[i][j] = ans[i-1][j-1] + ans[i-1][j];
        }
    }
    return ans;
}

int main()
{
    int numRows = 20; 
    int* returnSize;
    int** returnColumnSizes;

    // Allocate memory for returnSize and returnColumnSizes
    returnSize = malloc(sizeof(int));
    returnColumnSizes = malloc(sizeof(int*));

    // Call the generate function
    int** triangle = generate(numRows, returnSize, returnColumnSizes);

    // Print the generated triangle
    for (int i = 0; i < *returnSize; i++) {
        for (int j = 0; j < (*returnColumnSizes)[i]; j++) {
            printf("%d ", triangle[i][j]);
        }
        printf("\n");
    }

    // Free the allocated memory
    for (int i = 0; i < *returnSize; i++) {
        free(triangle[i]);
    }
    free(triangle);
    free(returnSize);
    free(returnColumnSizes);

    return 0;
}
