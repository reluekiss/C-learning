/* Network Delay Time
You are given a network of n directed nodes, labeled from 1 to n. You are also given times, a list of directed edges where times[i] = (ui, vi, ti).

ui is the source node (an integer from 1 to n)
vi is the target node (an integer from 1 to n)
ti is the time it takes for a signal to travel from the source to the target node (an integer greater than or equal to 0).
You are also given an integer k, representing the node that we will send a signal from.

Return the minimum time it takes for all of the n nodes to receive the signal. If it is impossible for all the nodes to receive the signal, return -1 instead.

Example 1:
- Input: times = [[1,2,1],[2,3,1],[1,4,4],[3,4,1]], n = 4, k = 1
- Output: 3

Example 2:
 - Input: times = [[1,2,1],[2,3,1]], n = 3, k = 2
 - Output: -1

Constraints:
 - 1 <= k <= n <= 100
 - 1 <= times.length <= 1000 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

typedef struct {
    int ui, vi, ti;
} Edge;

typedef struct {
    Edge* items;
    int count;
    int capacity;
} Times;

typedef struct {
    int v;
    int t;
} AdjEdge;

typedef struct {
    int node;
    int dist;
} HeapNode;

static void swap(HeapNode *a, HeapNode *b) {
    HeapNode tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heapPush(HeapNode heap[], int *size, HeapNode node) {
    int i = (*size)++;
    heap[i] = node;
    while(i > 0) {
        int parent = (i - 1) / 2;
        if(heap[parent].dist <= heap[i].dist) break;
        swap(&heap[parent], &heap[i]);
        i = parent;
    }
}

static HeapNode heapPop(HeapNode heap[], int *size) {
    HeapNode top = heap[0];
    heap[0] = heap[--(*size)];
    int i = 0;
    while(1) {
        int left = 2 * i + 1, right = 2 * i + 2, smallest = i;
        if(left < *size && heap[left].dist < heap[smallest].dist)
            smallest = left;
        if(right < *size && heap[right].dist < heap[smallest].dist)
            smallest = right;
        if(smallest == i) break;
        swap(&heap[i], &heap[smallest]);
        i = smallest;
    }
    return top;
}

int networkDelayTime(Times* times, int n, int k) {
    int m = times->count;
    int* edgeCount = calloc(n + 1, sizeof(int));
    for (int i = 0; i < m; i++)
        edgeCount[times->items[i].ui]++;

    AdjEdge** graph = malloc((n + 1) * sizeof(AdjEdge*));
    int* graphSize = calloc(n + 1, sizeof(int));
    for (int i = 1; i <= n; i++) {
        graph[i] = edgeCount[i] ? malloc(edgeCount[i] * sizeof(AdjEdge)) : NULL;
    }
    for (int i = 0; i < m; i++) {
        int u = times->items[i].ui, v = times->items[i].vi, t = times->items[i].ti;
        graph[u][graphSize[u]].v = v;
        graph[u][graphSize[u]++].t = t;
    }
    free(edgeCount);

    int* dist = malloc((n + 1) * sizeof(int));
    for (int i = 1; i <= n; i++) dist[i] = INT_MAX;
    dist[k] = 0;

    // Allocate heap with capacity >= m + n (upper bound)
    int heapCap = m + n;
    HeapNode* heap = malloc(heapCap * sizeof(HeapNode));
    int heapSize = 0;
    heapPush(heap, &heapSize, (HeapNode){k, 0});

    while(heapSize) {
        HeapNode cur = heapPop(heap, &heapSize);
        int u = cur.node, d = cur.dist;
        if(d > dist[u]) continue;
        if(graph[u]) {
            for (int i = 0; i < graphSize[u]; i++) {
                int v = graph[u][i].v, nd = d + graph[u][i].t;
                if(nd < dist[v]) {
                    dist[v] = nd;
                    heapPush(heap, &heapSize, (HeapNode){v, nd});
                }
            }
        }
    }

    int ans = 0;
    for (int i = 1; i <= n; i++) {
        if(dist[i] == INT_MAX) { ans = -1; break; }
        if(dist[i] > ans) ans = dist[i];
    }

    for (int i = 1; i <= n; i++) {
        if(graph[i]) free(graph[i]);
    }
    free(graph);
    free(graphSize);
    free(dist);
    free(heap);
    return ans;
}

int main() {
    // Example 1:
    Edge edges1[] = { {1,2,1}, {2,3,1}, {1,4,4}, {3,4,1} };
    Times times1 = { edges1, 4, 4 };
    int result1 = networkDelayTime(&times1, 4, 1);
    printf("Example 1 result: %d\n", result1); // Expected: 3

    // Example 2:
    Edge edges2[] = { {1,2,1}, {2,3,1} };
    Times times2 = { edges2, 2, 2 };
    int result2 = networkDelayTime(&times2, 3, 2);
    printf("Example 2 result: %d\n", result2); // Expected: -1

    return 0;
}
