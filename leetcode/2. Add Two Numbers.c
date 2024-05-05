/*
2. Add Two Numbers

You are given two non-empty linked lists representing two non-negative integers. The digits are stored in reverse order and each of their nodes contain a single digit. Add the two numbers and return it as a linked list.

You may assume the two numbers do not contain any leading zero, except the number 0 itself.


Input: (2 -> 4 -> 3) + (5 -> 6 -> 4)
Output: 7 -> 0 -> 8
*/
#include <stdio.h>
#include <stdlib.h>

struct ListNode {
    int val;
    struct ListNode *next;
}

struct ListNode* addtwonumbers(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode* result = malloc(sizeof(struct ListNode)); 
    struct ListNode* ptr = result;
    result->val = 0;
    result->next = NULL;

    int carry = 0;
    
    while (l1 != NULL || l2!=NULL || carry !=0){
        
        int a = (l1 == NULL) ? 0 : l1->val;
        int b = (l2 == NULL) ? 0 : l2->val;
        ptr->val = a + b + carry;
        carry = ptr->val / 10;
        ptr->val = ptr->val %10;
        
        if(l1!=NULL){
            l1 = l1->next;
        }
        
        if(l2 !=NULL){
            l2 = l2->next;
        }
        
        if(l1 != NULL || l2!=NULL || carry !=0){
            ptr->next = malloc(sizeof(struct ListNode));
            ptr->next->next=NULL;
            ptr = ptr->next;
        }
    }
    return result;
}

int main()
{
    int l1[] = {9,9,9,9,9,9,9}, l2[] = {9,9,9,9};
    printf("%s", addtwonumbers(l1, l2));
}
