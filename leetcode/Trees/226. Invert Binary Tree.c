/* Given the root of a binary tree, invert the tree, and return its root.

Example 1:
  Input: root = [4,2,7,1,3,6,9]
  Output: [4,7,2,9,6,3,1]

Example 2:
  Input: root = [2,1,3]
  Output: [2,3,1]

Example 3:
  Input: root = []
  Output: []
 
Constraints:
  The number of nodes in the tree is in the range [0, 100].
  -100 <= Node.val <= 100 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../include/btree.h"
#include "../../include/hashtable.h"

void atobt(TreeNode **t, void **a, int index, int n, int(cmp)(void*, int, int))
{
    int key = cmp(a[index], sizeof(a[index]), n);
    insertBtree2(*t, key, a);
    atobt(&(*t)->left, a, 2 * index + 1, n, cmp);
    atobt(&(*t)->right, a, 2 * index + 2, n, cmp);
}

void printbti(TreeNode *t, int level)
{
    if (t) {
        printbti(t->left, level + 1);
        printf("%*s->%d\n", 4*level, "", *(int*)t->val);
        printbti(t->right, level + 1);
    }
}

TreeNode* invertTree(TreeNode* root)
{
    void *tmp;
    while (root != NULL) {
        tmp = root->left;
        root->left = root->right;
        root->right = tmp;
    }
    return root;
}

int main()
{
    int treeArray[] = {0,1,2,3,4,5,6,7};
    int length = sizeof(treeArray)/sizeof(treeArray[0]);
    TreeNode *treeRoot;
    
    atobt(&treeRoot, (void *)treeArray, 0, length, default_hashf);
    treeRoot = invertTree(treeRoot);
    
    printbti(treeRoot, 0);
    
    clearTree(treeRoot);
}
