#ifndef _BFS_H_
#define _BFS_H_
#include "btree.h"
struct node { 
     int data; 
     struct node *next; 
};

extern void bfs(int adj[][10], int x,int visited[], int n, struct node **p); 

#endif
