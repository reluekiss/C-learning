#include <stdio.h>
#include <stdlib.h>
 
// Structure to represent each 
// node in a red-black tree
struct node {
    int d; // data
    int c; // 1-red, 0-black
    struct node* p; // parent
    struct node* r; // right-child
    struct node* l; // left child
};
 
// global root for the entire tree
struct node* root = NULL;
 
// function to perform BST insertion of a node
struct node* bst(struct node* trav, struct node* temp)
{
    // If the tree is empty, 
    // return a new node
    if (trav == NULL)
        return temp;
 
    // Otherwise recur down the tree
    if (temp->d < trav->d) 
    {
        trav->l = bst(trav->l, temp);
        trav->l->p = trav;
    }
    else if (temp->d > trav->d) 
    {
        trav->r = bst(trav->r, temp);
        trav->r->p = trav;
    }
 
    // Return the (unchanged) node pointer
    return trav;
}
 
// Function performing right rotation 
// of the passed node
void rightrotate(struct node* temp)
{
    struct node* left = temp->l;
    temp->l = left->r;
    if (temp->l)
        temp->l->p = temp;
    left->p = temp->p;
    if (!temp->p)
        root = left;
    else if (temp == temp->p->l)
        temp->p->l = left;
    else
        temp->p->r = left;
    left->r = temp;
    temp->p = left;
}
 
// Function performing left rotation 
// of the passed node
void leftrotate(struct node* temp)
{
    struct node* right = temp->r;
    temp->r = right->l;
    if (temp->r)
        temp->r->p = temp;
    right->p = temp->p;
    if (!temp->p)
        root = right;
    else if (temp == temp->p->l)
        temp->p->l = right;
    else
        temp->p->r = right;
    right->l = temp;
    temp->p = right;
}
 
// This function fixes violations 
// caused by BST insertion
void fixup(struct node* root, struct node* pt)
{
    struct node* p = NULL;
    struct node* g = NULL;
 
    while ((pt != root) && (pt->c != 0)
           && (pt->p->c == 1)) 
    {
        p = pt->p;
        g = pt->p->p;
 
        /*  Case : A
             Parent of pt is left child 
             of Grand-parent of
           pt */
        if (p == g->l) 
        {
 
            struct node* uncle_pt = g->r;
 
            /* Case : 1
                The uncle of pt is also red
                Only Recoloring required */
            if (uncle_pt != NULL && uncle_pt->c == 1) 
            {
                g->c = 1;
                p->c = 0;
                uncle_pt->c = 0;
                pt = g;
            }
 
            else {
 
                /* Case : 2
                     pt is right child of its parent
                     Left-rotation required */
                if (pt == p->r) {
                    leftrotate(p);
                    pt = p;
                    p = pt->p;
                }
 
                /* Case : 3
                     pt is left child of its parent
                     Right-rotation required */
                rightrotate(g);
                int t = p->c;
                p->c = g->c;
                g->c = t;
                pt = p;
            }
        }
 
        /* Case : B
             Parent of pt is right 
             child of Grand-parent of
           pt */
        else {
            struct node* uncle_pt = g->l;
 
            /*  Case : 1
                The uncle of pt is also red
                Only Recoloring required */
            if ((uncle_pt != NULL) && (uncle_pt->c == 1)) 
            {
                g->c = 1;
                p->c = 0;
                uncle_pt->c = 0;
                pt = g;
            }
            else {
                /* Case : 2
                   pt is left child of its parent
                   Right-rotation required */
                if (pt == p->l) {
                    rightrotate(p);
                    pt = p;
                    p = pt->p;
                }
 
                /* Case : 3
                     pt is right child of its parent
                     Left-rotation required */
                leftrotate(g);
                int t = p->c;
                p->c = g->c;
                g->c = t;
                pt = p;
            }
        }
    }
}
 
// Function to print inorder traversal 
// of the fixated tree
void inorder(struct node* trav)
{
    if (trav == NULL)
        return;
    inorder(trav->l);
    printf("%d ", trav->d);
    inorder(trav->r);
}
 
// driver code
int main()
{
    int n = 7;
    int a[7] = { 7, 6, 5, 4, 3, 2, 1 };
 
    for (int i = 0; i < n; i++) {
 
        // allocating memory to the node and initializing:
        // 1. color as red
        // 2. parent, left and right pointers as NULL
        // 3. data as i-th value in the array
        struct node* temp = (struct node*)malloc(sizeof(struct node));
        temp->r = NULL;
        temp->l = NULL;
        temp->p = NULL;
        temp->d = a[i];
        temp->c = 1;
 
        // calling function that performs bst insertion of
        // this newly created node
        root = bst(root, temp);
 
        // calling function to preserve properties of rb
        // tree
        fixup(root, temp);
          root->c = 0;
    }
 
    printf("Inorder Traversal of Created Tree\n");
    inorder(root);
 
    return 0;
}
