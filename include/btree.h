#ifndef BTREE_H
#define BTREE_H
#include <stdio.h>

// Define a structure to be used as the tree node
typedef struct TreeNodeType {
    int                 freq;
    int                 key;
    void                *val;
    struct TreeNodeType *left, *right;
}TreeNode;

typedef struct BTree {
    int      size;
    TreeNode root;
}BTree;

// Function prototypes
extern void clearTree(TreeNode *T);
extern TreeNode *searchTree(BTree *T, int key);
extern int insertBtree1(BTree *T, TreeNode *newNode);
extern int insertBtree2(BTree *T, int key, void *val);
extern int deleteBtnode(BTree *T, int key);
extern void foreachBtree(BTree *T,  void(*f)(TreeNode*));
extern int btbfs(BTree *T);
extern TreeNode *swapBtree(TreeNode *p);

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif

#endif
