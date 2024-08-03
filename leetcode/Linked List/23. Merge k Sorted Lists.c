/* You are given an array of k linked-lists lists, each linked-list is sorted in ascending order.

Merge all the linked-lists into one sorted linked-list and return it.

Example 1:

Input: lists = [[1,4,5],[1,3,4],[2,6]]
  Output: [1,1,2,3,4,4,5,6]
  Explanation: The linked-lists are:
  [
    1->4->5,
    1->3->4,
    2->6
  ]
  merging them into one sorted list:
  1->1->2->3->4->4->5->6

Example 2:
  Input: lists = []
  Output: []

Example 3:
  Input: lists = [[]]
  Output: []

Constraints:
 - k == lists.length
 - 0 <= k <= 10^4
 - 0 <= lists[i].length <= 500
 - -10^4 <= lists[i][j] <= 10^4
 - lists[i] is sorted in ascending order.
 - The sum of lists[i].length will not exceed 10^4. */

#include "llist.h"
#include <stdio.h>
#include <stdlib.h>

int cmp(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}

void ksll(struct llist llistArray[], struct llist outllist)
{
	int outArray[10000];
	for(int i = 0; i < 500; i++){
		int tmplength = llistArray[i].count;
		struct llist_node *current = llistArray[i].head;
		for(int j = 0; j < tmplength;) {
			outArray[j] = *(int*)current->data;
			current = current->next;
			j = i*tmplength + j;
		}
	}
	qsort(outArray, 10000, sizeof(int), cmp);
	for(int j = 0; j < 500; j++){
		llist_append(&outllist, &outArray[j]);
	}
}

int main()
{
	int outArray[10000];
	struct llist *llistArray[500];
	llistArray[0] = llist_create();
	int lists[3][3] = {{1,2,4},{1,3,5},{3,6}};
	

	for(int i = 0; i < 500; i++){
		for(int j = 0; j < 500; j++){
			llist_append(llistArray[i], &lists[i][j]);	
		}
	}

	ksll(*llistArray, outArray);
	
	for(int i = 0; i < 10000; i++){
		printf("%d, ", outArray[i]);
	}
	for(int i = 0; i < 500; i++){
		llist_destroy(llistArray[i]);
	}
}
