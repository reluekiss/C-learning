#ifndef BTREE_H
#define BTREE_H
#include <stdio.h>

// Define a structure to be used as the tree node
typedef struct TreeNodeType
{
    int                 key;
    void                *val;
    struct TreeNodeType *left;
    struct TreeNodeType *right;
}TreeNode;

// Function prototypes
extern void clearTree(TreeNode *T);
extern TreeNode *searchTree(TreeNode *root, int key);
extern TreeNode *dupNode(TreeNode * T);
extern int insertBtree1(TreeNode *root, TreeNode *newNode);
extern int insertBtree2(TreeNode *root, int key, void *val);
extern int deleteBtnode(TreeNode *root, int key);

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif

#endif
