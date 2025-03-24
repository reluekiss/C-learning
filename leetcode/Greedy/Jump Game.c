/* Jump Game
You are given an integer array nums where each element nums[i] indicates your maximum jump length at that position.

Return true if you can reach the last index starting from index 0, or false otherwise.

Example 1:
 - Input: nums = [1,2,0,1,0]
 - Output: true

Explanation: First jump from index 0 to 1, then from index 1 to 3, and lastly from index 3 to 4.

Example 2:
 - Input: nums = [1,2,1,0,1]
 - Output: false

Constraints:
 - 1 <= nums.length <= 1000
 - 0 <= nums[i] <= 1000 */

#include <stdio.h>
int canJump(int* nums, int size) {
    int goal = size - 1;
    for (int i = size - 2; i >= 0; i--) {
        if (i + nums[i] >= goal)
            goal = i;
    }
    return goal == 0;
}

int main() {
    int nums[] = {1,2,0,1,0};
    printf("Example 1: %d\n", canJump(nums, 5));
    int nums1[] = {1,2,1,0,1};
    printf("Example 2: %d\n", canJump(nums1, 5));
}
