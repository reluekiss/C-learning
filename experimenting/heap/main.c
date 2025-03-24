#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(void *a, void *b, size_t size) {
    unsigned char *p = a, *q = b, tmp;
    for (size_t i = 0; i < size; i++) {
        tmp = p[i];
        p[i] = q[i];
        q[i] = tmp;
    }
}

typedef struct {
    void   *items;
    size_t  count;
    size_t  capacity;
    size_t  elem_size;
    int   (*cmp)(const void *, const void *);
} Heap;

void heap_push(Heap *heap, const void *item) {
    if (heap->count >= heap->capacity) exit(EXIT_FAILURE);
    char *base = heap->items;
    memcpy(base + heap->count * heap->elem_size, item, heap->elem_size);
    size_t n = heap->count;
    heap->count++;
    while (n > 0) {
        size_t p = (n - 1) / 2;
        if (heap->cmp(base + p * heap->elem_size, base + n * heap->elem_size) >= 0)
            break;
        swap(base + p * heap->elem_size, base + n * heap->elem_size, heap->elem_size);
        n = p;
    }
}

void heap_pop(Heap *heap, void *out) {
    assert(heap->count > 0);
    char *base = heap->items;
    memcpy(out, base, heap->elem_size);
    heap->count--;
    memcpy(base, base + heap->count * heap->elem_size, heap->elem_size);
    size_t n = 0;
    while (1) {
        size_t l = 2 * n + 1, r = 2 * n + 2, m = n;
        if (l < heap->count && heap->cmp(base + l * heap->elem_size, base + m * heap->elem_size) > 0)
            m = l;
        if (r < heap->count && heap->cmp(base + r * heap->elem_size, base + m * heap->elem_size) > 0)
            m = r;
        if (m == n) break;
        swap(base + n * heap->elem_size, base + m * heap->elem_size, heap->elem_size);
        n = m;
    }
}

void heap_sort(Heap heap) {
    for (size_t i = 0; i < heap.count; i++) {
        heap_push(&heap, (char*)heap.items + i * heap.elem_size);
    }
    for (size_t i = heap.count; i > 0; i--) {
        heap_pop(&heap, (char*)heap.items + (i - 1) * heap.elem_size);
    }
}

void heap_traverse(Heap *heap, size_t node, int depth, void (*print)(const void *, int)) {
    if (node >= heap->count) return;
    size_t l = 2 * node + 1, r = 2 * node + 2;
    heap_traverse(heap, l, depth + 1, print);
    print((char*)heap->items + node * heap->elem_size, depth);
    heap_traverse(heap, r, depth + 1, print);
}

void print_int(const void *data, int depth) {
    printf("%*s%d\n", 3 * depth, "", *(const int *)data);
}

int int_cmp(const void *a, const void *b) {
    return (*(const int *)a) - (*(const int *)b);
}

int main(void) {
    int arr[] = { 5, 3, 8, 4, 1, 9, 2, 7, 6, 0, 1234, 43120 };
    size_t n = sizeof(arr) / sizeof(arr[0]);

    Heap heap = {
        .items = arr,
        .count = 0,
        .capacity = n,
        .elem_size = sizeof(int),
        .cmp = int_cmp,
    };
    for (size_t i = 0; i < n; i++) {
        heap_push(&heap, &arr[i]);
    }
    printf("Heap traverse:\n");
    heap_traverse(&heap, 0, 0, print_int);

    heap_sort(heap);
    printf("Sorted array:\n");
    heap_traverse(&heap, 0, 0, print_int);

    free(heap.items);
    return 0;
}
