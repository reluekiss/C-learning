/* Given an integer array nums of unique elements, return all possible 
subsets (the power set).

The solution set must not contain duplicate subsets. Return the solution in any order.

Example 1:
  Input: nums = [1,2,3]
  Output: [[],[1],[2],[1,2],[3],[1,3],[2,3],[1,2,3]]

Example 2:
  Input: nums = [0]
  Output: [[],[0]]
 
Constraints:
  1 <= nums.length <= 10
  -10 <= nums[i] <= 10
  All the numbers of nums are unique. */

// Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
int** subsets(int* nums, int numsSize, int* returnSize, int** returnColumnSizes)
{
    int ans[*returnSize][**returnColumnSizes];
    for(int i = 0; i < (1 << numsSize); i++){
        int subset[**returnColumnSizes];
        for(int j = 0; j < numsSize; i++){
            if((i & (1 << i)) != 0)
                subset[j] = nums[j];
        }
        ans[i] = subset;
    }
    return ans;
}
