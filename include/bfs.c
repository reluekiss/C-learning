#include "llist.h"
#include "btree.h"
#include <bits/types/stack_t.h>
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>

// Function to remove an element from the queue
void *pop(struct llist* q) {
    if (q->head == NULL) {
        printf("struct llist is Empty\n");
        return FALSE;
    }
    void *data = q->head->data;
    q->head = q->head->next;
    q->count--;
    return data;
}

// Breadth first search
int bfs(TreeNode* root)
{
    int level = 0;
    TreeNode* node = root;
    struct llist* q = llist_create();
    
    if (root == NULL) {
        fprintf(stderr, "root tree node is null");
        exit(-1);
    }
    llist_append(q, root);

    while (q->head != NULL) {
        for (int i = 0; i < q->count; i++) {
            pop(q);
            if (node->left != NULL) {
                llist_append(q, node->left);
            }
            if (node->right != NULL) {
                llist_append(q, node->right);
            }
        }
        level++;
    }
    llist_destroy(q);
    return level;
}
