#include "bfs.h"
#include <stdio.h>
#include <stdlib.h>

// TODO: see if this is possible with a sparse matrix linked list

// A function to insert a new node in queue 
struct node *addqueue(struct node *p,int val) 
{ 
    struct node *temp; 
    if(p == NULL) { 
        p = (struct node *) malloc(sizeof(struct node));
        if(p == NULL) { 
            printf("Cannot allocate\n"); 
            exit(0); 
        }
        p->data = val; 
        p->next=NULL; 
    } else { 
        temp= p; 
        while(temp->next != NULL) { 
           temp = temp->next; 
        } 
        temp->next = (struct node*)malloc(sizeof(struct node)); 
        temp = temp->next; 
        if(temp == NULL) { 
            printf("Cannot allocate\n"); 
            exit(0); 
        } 
    temp->data = val; 
    temp->next = NULL; 
    } 
    return(p); 
} 

struct node *deleteq(struct node *p,int *val) 
{
    struct node *temp; 
    if(p == NULL) { 
        printf("queue is empty\n"); 
        return(NULL); 
    } 
    *val = p->data; 
    temp = p; 
    p = p->next; 
    free(temp); 
    return(p); 
} 

// Breadth first search for a graph in adjacent matrix form.
void bfs(int adj[][10], int x,int visited[], int n, struct node **p) 
{ 
    int y,j,k; 
    *p = addqueue(*p,x); 
    while((*p) != NULL) { 
        *p = deleteq(*p,&y); 
        if(visited[y] == 0) { 
            printf("\nnode visited = %d\t",y); 
            visited[y] = 1; 
            for(j=0;j<n;j++) {
                if((adj[y][j] ==1) && (visited[j] == 0))
                    *p = addqueue(*p,j); 
            }
        } 
    } 
} 

// Recursive depth first seach for adjacency matrix graph
void dfs(int x,int visited[],int adj[][10],int n) 
{ 
   int j; 
   visited[x] = 1; 
   printf("The node visited id %d\n",x); 
   for(j=0;j<n;j++) 
      if(adj[x][j] ==1 && visited[j] ==0) 
           dfs(j,visited,adj,n); 
} 
