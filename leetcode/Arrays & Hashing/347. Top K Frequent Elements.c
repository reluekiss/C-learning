/* Top K Elements in List
Given an integer array nums and an integer k, return the k most frequent elements within the array.

The test cases are generated such that the answer is always unique.

You may return the output in any order.

Example 1:

Input: nums = [1,2,2,3,3,3], k = 2

Output: [2,3]
Example 2:

Input: nums = [7,7], k = 1

Output: [7]
Constraints:

1 <= nums.length <= 10^4.
-1000 <= nums[i] <= 1000
1 <= k <= number of distinct elements in nums. */
#include "hashtable.h"
#include <stdlib.h>

int topKfrequent(int *list[], int *k)
{
    int listLength = sizeof(*list) / sizeof(list[0]);
    struct hashtable *ht = hashtable_create(listLength, NULL);
    
    for(int i = 0; i < listLength; i++) {
        if(hashtable_get_bin(ht, list[i], 1) == NULL)
            hashtable_put_bin(ht, list[i], 1, 1);
        ht->
    }
    
    hashtable_foreach(*ht, hashtable_get_bin(*ht, key, int key_size), void *arg)

    int a[*k];
    for(int i = 0; i < *k; i++){
        a[i] = 
    }
    return a;    
}
