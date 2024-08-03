/* Given a string s containing just the characters '(', ')', '{', '}', '[' and ']', determine if the input string is valid.

An input string is valid if:

Open brackets must be closed by the same type of brackets.
Open brackets must be closed in the correct order.
Every close bracket has a corresponding open bracket of the same type.
 

Example 1:

Input: s = "([{}])"
Output: true
Example 2:

Input: s = "()[]{}"
Output: true
Example 3:

Input: s = "[(])"
Output: false
 

Constraints:

1 <= s.length <= 104
s consists of parentheses only '()[]{}'. */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../../include/hashtable.h"

#define MAX_SIZE 128

typedef struct {
	int top;
	int items[MAX_SIZE];
} Stack;

void push(Stack* stack, int item)
{
	if(!(stack->top == MAX_SIZE - 1)) {
		stack->items[(stack->top)++] = item;
	} else {
		fprintf(stderr, "Stack is full\n");
	}
}

int pop(Stack* stack)
{
	if(!(stack->top == -1)) {
		return stack->items[(stack->top--)];
	} else {
		fprintf(stderr, "Stack is empty\n");
		return -1;
	}
}

struct table {
	char *a;
	char *b;
};

int isValid(char* s)
{
	struct table table[] = {
		{"(" , ")"},
		{"[" , "]"},
		{"(" , ")"}
	};
		
	Stack* stack;
	stack->top = -1;
	
	struct hashtable *ht = hashtable_create(strlen(s), NULL);
	hashtable_put(ht, table->a, table->b);
	for(int i = 0; i < strlen(s); i++)
		stack->items[i] = s[i];
	
	
	return ;
}
