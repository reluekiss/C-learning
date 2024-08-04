#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"
#include "llist.h"

// Purpose: Perform a recursive traversal of
//        a tree, removing all descendents from specified node.  
// Note: Function is declared static so it cannot
//        be accessed from outside                      
void clearTree(TreeNode *T)
{
    if(T==NULL) return;  // Nothing to clear
    if(T->left != NULL) clearTree(T->left); // Clear left sub-tree
    if(T->right != NULL) clearTree(T->right); // Clear right sub-tree
    free(T);    // Destroy this node
    return;
}

// Purpose: Perform an iterative search of the tree and     
//        return a pointer to a treenode containing the  
//        search key or NULL if not found.               
// Preconditions: None
TreeNode *searchTree(BTree *T, int key)
{
    int      ValueInTree = FALSE;
    TreeNode *temp;

    temp = &T->root;
    while((temp != NULL) && (temp->key != key))
    {
        if(key < temp->key)
            temp = temp->left;  // Search key comes before this node.
        else
            temp = temp->right; // Search key comes after this node 
    }
    if(temp == NULL) return temp;    // Search key not found
    else{
        temp->freq++;
        return(temp);    // Found it so return a duplicate
    }
}

// Insert a new node into the tree.                
// Preconditions: None
// Returns: int (TRUE if successful, FALSE otherwise)
int insertBtree1(BTree *T, TreeNode *newNode)
{
    TreeNode *temp;
    TreeNode *back;

    temp = &T->root;
    back = NULL;

    while(temp != NULL) // Loop till temp falls out of the tree 
    {
        back = temp;
        if(newNode->key < temp->key)
            temp = temp->left;
        else
            temp = temp->right;
    }

    // Now attach the new node to the node that back points to 
    if(back == NULL) // Attach as root node in a new tree 
        T->root = *newNode;
    else
    {
        if(newNode->key < back->key)
            back->left = newNode;
        else
            back->right = newNode;
    }
   T->size++;
   return(TRUE);
}

// Insert a new node into the tree.                
// Preconditions: None
// Returns: int (TRUE if successful, FALSE otherwise)
int insertBtree2(BTree *T, int key, void *val)
{
    TreeNode *newNode;

    // Create the new node and copy data into it
    newNode = (TreeNode *)malloc(sizeof(TreeNode));
    newNode->key = key;
    newNode->val = val;
    newNode->left = newNode->right = NULL;

    // Call Insert1() to do the actual insertion
    return(insertBtree1(T, newNode));
}

// Purpose: Delete a node from the tree.                    
// Preconditions: Tree contains the node to delete
// Returns: int (TRUE if successful, FALSE otherwise)                               
int deleteBtnode(BTree *T, int key)
{
    TreeNode *back;
    TreeNode *temp;
    TreeNode *delParent;    // Parent of node to delete
    TreeNode *delNode;      // Node to delete
    TreeNode *root = &T->root;
    temp = root;
    back = NULL;

    // Find the node to delete 
    while((temp != NULL) && (key != temp->key)) {
        back = temp;
        if(key < temp->key)
            temp = temp->left;
        else
            temp = temp->right;
    }

    if(temp == NULL) {
        printf("Key not found. Nothing deleted.\n");
        return FALSE;
    }
    else {
        if(temp == root) {
            delNode = root;
            delParent = NULL; 
        } else {
            delNode = temp;
            delParent = back;
        }
    }

    // Case 1: Deleting node with no children or one child 
    if(delNode->right == NULL) {
        if(delParent == NULL) {
            root = delNode->left;
            free(delNode);
            return TRUE;
        }
        else {
            if(delParent->left == delNode)
                delParent->left = delNode->left;
            else
                delParent->right = delNode->left;
            free(delNode);
            return TRUE;
        }
    }
    else {
        if(delNode->left == NULL) {
            if(delParent == NULL) {
                root = delNode->right;
                free(delNode);
                return TRUE;
            }
            else {
                if(delParent->left == delNode)
                    delParent->left = delNode->right;
                else
                    delParent->right = delNode->right;
                free(delNode);
                return TRUE;
            }
        }
        // Case 2: Deleting node with two children 
        else {
            // Find the replacement value.  Locate the node  
            // containing the largest value smaller than the 
            // key of the node being deleted.                
            temp = delNode->left;
            back = delNode;
            while(temp->right != NULL) {
                back = temp;
                temp = temp->right;
            }
            // Copy the replacement values into the node to be deleted 
            delNode->key = temp->key;
            delNode->val = temp->val;

            // Remove the replacement node from the tree 
            if(back == delNode)
                back->left = temp->left;
            else
                back->right = temp->left;
            free(temp);
            return TRUE;
        }
    }
}

// Recursively swap the children of a node.
TreeNode *swapBtree(TreeNode *p) 
{
    TreeNode *temp1=NULL, *temp2=NULL; 
    if(p != NULL) { 
        temp1= swapBtree(p->left); 
        temp2 = swapBtree(p->right);
        p->right = temp1; 
        p->left = temp2; 
     } 
     return(p); 
} 

// Iterative inorder traversal, upon which freq and key is put in matrix.
void foreachBtree(BTree *T,  void(*f)(TreeNode*))
{
    TreeNode *p = &T->root; 
    TreeNode *stack[100]; 
    int i = 0;
    int top;
    top = -1; 
    if(p != NULL) {
        top++; 
        stack[top] = p; 
        p = p->left; 
        while(top >= 0) { 
             while (p!= NULL) { 
                top++; 
                stack[top] = p; 
                p = p->left; 
            }  
           
            p = stack[top]; 
            top--;
            f(p);
            p = p->right; 

            if (p != NULL) { 
                top++; 
                stack[top] = p; 
                p = p->left; 
            } 
        } 
    }
}

int *order(int *best[], int i, int j)
{
    if (i == j) {
        order(best, i, best[i][j]-1);
        order(best, best[i][j], j);
    }
}

// Create an optimised binary search tree depending on access frequency.
void obst(BTree *T)
{
    int N = T->size;
    int knf[N][2];
    ree(T, knf);
    int cost[N+1][N],best[N+1][N];
    int i,j,k,t;

    for (i = 1; i <= N; i++)
        for (j = i+1; j <= N; j++) 
            cost[i][j] = INT_MAX;
    for (i = 1; i <= N; i++) 
        cost[i][i] = knf[i][1];
    for (i = 1; i <= N+1; i++) 
        cost[i][i-1] = 0;
    for (j = 1; i <= N-1; j++)
        for (i = 1; i <= N-j; i++)
            for (k = i; k <= i+j; k++) {
                t = cost[i][k-1] + cost[k+1][i+j];
                if (t < cost[i][i+j]) {
                    cost[i][i+j] = t;
                    best[i][i+j] = k;
                }
            }
            for (k = i; k <= i+j; cost[i][j] += knf[k++][1]);
    i = j = 0;
    order(best, i, j);
}

// Function to remove an element from the queue
void *pop(struct llist* q) {
    if (q->head == NULL) {
        printf("struct llist is Empty\n");
        return FALSE;
    }
    void *data = q->head->data;
    free(q->head);
    q->head = q->head->next;
    q->count--;
    return data;
}

// Breadth first search for a binary tree, using a linked list queue.
int btbfs(BTree *T)
{
    int level = 0;
    TreeNode *node, *root = &T->root;
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
