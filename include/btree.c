#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

// Purpose: Perform a recursive traversal of
//        a tree destroying all nodes.  
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

// Purpose: Duplicate a node in the tree.  This
//        is used to allow returning a complete
//        structure from the tree without giving
//        access into the tree through the pointers.               
// Returns: Pointer to a duplicate of the node arg
// Note: Function is declared static so it cannot
//        be accessed from outside                      
TreeNode *dupNode(TreeNode * T)
{
    TreeNode *dupNode;

    dupNode = (TreeNode *)malloc(sizeof(TreeNode));
    *dupNode = *T;    // Copy the data structure
    dupNode->left = NULL;    // Set the pointers to NULL
    dupNode->right = NULL;
    return dupNode;
}

// Purpose: Perform an iterative search of the tree and     
//        return a pointer to a treenode containing the  
//        search key or NULL if not found.               
// Preconditions: None
// Returns: Pointer to a duplicate of the node found
TreeNode *searchTree(TreeNode *root, int key)
{
    int      ValueInTree = FALSE;
    TreeNode *temp;

    temp = root;
    while((temp != NULL) && (temp->key != key))
    {
        if(key < temp->key)
            temp = temp->left;  // Search key comes before this node.
        else
            temp = temp->right; // Search key comes after this node 
    }
    if(temp == NULL) return temp;    // Search key not found
    else
        return(dupNode(temp));    // Found it so return a duplicate
}

// Insert a new node into the tree.                
// Preconditions: None
// Returns: int (TRUE if successful, FALSE otherwise)
int insertBtree1(TreeNode *root, TreeNode *newNode)
{
    TreeNode *temp;
    TreeNode *back;

    temp = root;
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
        root = newNode;
    else
    {
        if(newNode->key < back->key)
            back->left = newNode;
        else
            back->right = newNode;
    }
   return(TRUE);
}

// Insert a new node into the tree.                
// Preconditions: None
// Returns: int (TRUE if successful, FALSE otherwise)
int insertBtree2(TreeNode *root, int key, void *val)
{
    TreeNode *newNode;

    // Create the new node and copy data into it
    newNode = (TreeNode *)malloc(sizeof(TreeNode));
    newNode->key = key;
    newNode->val = val;
    newNode->left = newNode->right = NULL;

    // Call Insert1() to do the actual insertion
    return(insertBtree1(root, newNode));
}

// Purpose: Delete a node from the tree.                    
// Preconditions: Tree contains the node to delete
// Returns: int (TRUE if successful, FALSE otherwise)                               
int deleteBtnode(TreeNode *root, int key)
{
    TreeNode *back;
    TreeNode *temp;
    TreeNode *delParent;    // Parent of node to delete
    TreeNode *delNode;      // Node to delete

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
