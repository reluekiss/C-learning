#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#define N 10000
#define BUF_SIZE (N * sizeof(int))

typedef struct {
    char* buffer;     // pointer to a 2x-mapped region
    size_t size;     // number of elements (N)
    size_t head;     // absolute write index
    size_t tail;     // absolute read index
} ring_buffer;

/* Create a circular buffer with a double mapping */
ring_buffer *ring_buffer_init(size_t count) {
    /* Page aligned size */
    size_t buf_size = count * sizeof(int);
    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    size_t size = ((buf_size - 1) / pagesize + 1) * pagesize;

    /* Create a POSIX shared memory object */
    int fd = shm_open("/ringbuffer_shm", O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    shm_unlink("/ringbuffer_shm");
    if (ftruncate(fd, size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    /* Reserve contiguous virtual address space for 2*size */
    void *addr = mmap(NULL, size * 2, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap reserve");
        exit(EXIT_FAILURE);
    }
    /* Map the shared memory into the first half */
    void *addr1 = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0);
    if (addr1 == MAP_FAILED) {
        perror("mmap first half");
        exit(EXIT_FAILURE);
    }
    /* Map the same shared memory into the second half */
void *addr2 = mmap((char*)addr + size, size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0);
    if (addr2 == MAP_FAILED) {
        perror("mmap second half");
        exit(EXIT_FAILURE);
    }
    close(fd);

    ring_buffer *rb = calloc(1, sizeof(ring_buffer));
    if (!rb) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    rb->buffer = (char*)addr;
    rb->size = count;
    rb->head = 0;
    rb->tail = 0;
    return rb;
}

void ring_buffer_destroy(ring_buffer *rb) {
    munmap(rb->buffer, BUF_SIZE * 2);
    free(rb);
}

/*
 * Note:
 *   Using double mapping, we can use the absolute indices directly.
 *   The buffer appears contiguous over the range [buffer, buffer + 2*count).
 *   Full condition: (head - tail) == count.
 */

int ring_buffer_put(ring_buffer *rb, char item) {
    if (rb->head - rb->tail >= rb->size)
        return 0;  // Buffer full

    rb->buffer[rb->head] = item;
    rb->head++;
    return 1;
}

int ring_buffer_get(ring_buffer *rb, char* item) {
    if (rb->head == rb->tail)
        return 0;  // Buffer empty

    *item = rb->buffer[rb->tail];
    rb->tail++;
    return 1;
}

int main(void) {
    ring_buffer *rb = ring_buffer_init(N);
    int result, value;
    
    // Initially empty: get should fail.
    result = ring_buffer_get(rb, &value);
    assert(result == 0);
    
    // Fill the buffer with (N - 1) items.
    for (int i = 0; i < N; i++) {
        result = ring_buffer_put(rb, i);
        assert(result == 1);
    }
    
    // Buffer full: next ring_buffer_put should fail.
    result = ring_buffer_put(rb, 100);
    assert(result == 0);
    
    // Retrieve all items and verify order.
    for (int i = 0; i < N; i++) {
        result = ring_buffer_get(rb, &value);
        assert(result == 1);
        assert(value == i);
    }
    
    // Buffer empty: ring_buffer_get should fail.
    result = ring_buffer_get(rb, &value);
    assert(result == 0);

    ring_buffer_destroy(rb);
    return 0;
}
