#ifndef R_BUFF_H_
#define R_BUFF_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#define N 4096*4096
#define BUF_SIZE (4096 * 4096 * sizeof(char))

typedef struct {
    char* buffer;     // pointer to a 2x-mapped region
    size_t size;     // number of elements (N)
    size_t head;     // absolute write index
    size_t tail;     // absolute read index
} RingBuffer;

RingBuffer *RingBuffer_init();
void RingBuffer_destroy(RingBuffer *rb);
int RingBuffer_put(RingBuffer *rb, int item);
int RingBuffer_get(RingBuffer *rb, int item);
int RingBuffer_read(RingBuffer *buffer, char *target, int amount);
int RingBuffer_write(RingBuffer *buffer, char *data, int length);
int RingBuffer_empty(RingBuffer *buffer);
int RingBuffer_full(RingBuffer *buffer);
int RingBuffer_available_data(RingBuffer *buffer);
int RingBuffer_available_space(RingBuffer *buffer);

#define RingBuffer_available_data(B) (((B)->end + 1) % (B)->length - (B)->start - 1)
#define RingBuffer_available_space(B) ((B)->length - (B)->end - 1)
#define RingBuffer_full(B) (RingBuffer_available_data((B)) - (B)->length == 0)
#define RingBuffer_empty(B) (RingBuffer_available_data((B)) == 0)
#define RingBuffer_puts(B, D) RingBuffer_write((B), bdata((D)), blength((D)))
#define RingBuffer_get_all(B) RingBuffer_gets((B), RingBuffer_available_data((B)))
#define RingBuffer_starts_at(B) ((B)->buffer + (B)->start)
#define RingBuffer_ends_at(B) ((B)->buffer + (B)->end)
#define RingBuffer_commit_read(B, A) ((B)->start = ((B)->start + (A)) % (B)->length)

#endif // R_BUFF_H_

#ifdef R_BUFF_IMPLEMENTATION

/* Create a circular buffer with a double mapping */
RingBuffer *RingBuffer_init() {
    /* Page aligned size */
    size_t buf_size = BUF_SIZE;
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

    RingBuffer *rb = calloc(1, sizeof(RingBuffer));
    if (!rb) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    rb->buffer = (char *)addr;
    rb->size = N;
    rb->head = 0;
    rb->tail = 0;
    return rb;
}

void RingBuffer_destroy(RingBuffer *rb) {
    munmap(rb->buffer, BUF_SIZE * 2);
    free(rb);
}

/*
 * Note:
 *   Using double mapping, we can use the absolute indices directly.
 *   The buffer appears contiguous over the range [buffer, buffer + 2*count).
 *   Full condition: (head - tail) == count.
 */

int RingBuffer_put(RingBuffer *rb, char item) {
    if (rb->head - rb->tail >= rb->size)
        return 0;  // Buffer full

    rb->buffer[rb->head] = item;
    rb->head++;
    return 1;
}

int RingBuffer_get(RingBuffer *rb, char* item) {
    if (rb->head == rb->tail)
        return 0;  // Buffer empty

    *item = rb->buffer[rb->tail];
    rb->tail++;
    return 1;
}

int RingBuffer_read(RingBuffer *rb, char *target, int amount) {
    int available = rb->head - rb->tail;
    int to_read = (amount > available) ? available : amount;
    if (to_read <= 0)
        return 0;
    memcpy(target, rb->buffer + rb->tail, to_read);
    rb->tail += to_read;
    return to_read;
}

int RingBuffer_write(RingBuffer *rb, char *data, int length) {
    int available = rb->size - (rb->head - rb->tail);
    int to_write = (length > available) ? available : length;
    if (to_write <= 0)
        return 0;
    memcpy(rb->buffer + rb->head, data, to_write);
    rb->head += to_write;
    return to_write;
}

#endif // R_BUFF_IMPLEMENTATION
