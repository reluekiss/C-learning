/*
** Given an integer array nums, return true if any value appears at least twice in the array, and return false if every element is distinct.
** 
** Example 1:
** 
** Input: nums = [1,2,3,1]
** >Output: true
** >Example 2:
** 
** Input: nums = [1,2,3,4]
** >Output: false
** >Example 3:
** 
** Input: nums = [1,1,1,3,3,4,3,2,4,2]
** >Output: true
**  
** Constraints:
** 1 <= nums.length <= 10^5
** -10^9 <= nums[i] <= 10^9
*/

/*
    Time: O(n)
    Space: O(1)
*/

#include <stdio.h>
#include <stdbool.h>
#include <uthash.h>

typedef struct {
	int key;
	UT_hash_handle hh; 
} hash_table;

hash_table *hash = NULL, *elem, *tmp;

bool containsDuplicate(int* nums, int numsSize){
    if (numsSize == 1) {
      return false;
    }
    
    bool flag = false;
    
    for (int i=0; i < numsSize; i++) {
        HASH_FIND_INT(hash, &nums[i], elem);
        
        if(!elem) {
           elem = malloc(sizeof(hash_table));
           elem->key = nums[i];
           HASH_ADD_INT(hash, key, elem);
            
           flag = false;
	}
	else {
           flag = true;
           break;
       }
    }
                          
	HASH_ITER(hh, hash, elem, tmp) {
		HASH_DEL(hash, elem); free(elem);
	}

    return flag;
}

int main()
{
    int nums[] = {1,1,1,3,3,4,3,2,4,2};
    int numsSize = sizeof(nums)/sizeof(nums[0]);
    printf("%b\n", containsDuplicate(nums, numsSize));
}
