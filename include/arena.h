#ifndef ARENA_H_
#define ARENA_H_

#include <stddef.h>
#include <stdint.h>

#ifndef ARENA_ASSERT
#include <assert.h>
#define ARENA_ASSERT assert
#endif

typedef struct Region Region;

struct Region {
    Region *next;
    size_t count;
    size_t capacity;
    uintptr_t data[];
};

typedef struct {
    Region *begin, *end;
    size_t new_region_calls;
    size_t existing_region_skipped;
    size_t allocation_exceeded;
} Arena;

#define REGION_DEFAULT_CAPACITY (8*1024)

Region *new_region(size_t capacity);
void free_region(Region *r);

// TODO: snapshot/rewind capability for the arena
// - Snapshot should be combination of a->end and a->end->count. 
// - Rewinding should be restoring a->end and a->end->count from the snapshot and
// setting count-s of all the Region-s after the remembered a->end to 0.
void *arena_alloc(Arena *a, size_t size_bytes);
void arena_reset(Arena *a);
void arena_free(Arena *a);
void print_arena_statistics(); 

#endif // ARENA_H_

#ifdef ARENA_IMPLEMENTATION
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#define MAP_ANONYMOUS 0x20
#define MAP_PRIVATE 0x02

Region *new_region(size_t object_size)
{
    size_t capacity = (object_size + sizeof(Region)) / sizeof(void*);

    size_t size_bytes = sizeof(Region) + sizeof(void*)*capacity;

    size_t alignment = (sizeof(Region) + REGION_DEFAULT_CAPACITY - 1) & ~(REGION_DEFAULT_CAPACITY - 1);
    size_t padded_size = alignment + (size_bytes % alignment == 0 ? 0 : (REGION_DEFAULT_CAPACITY - size_bytes % REGION_DEFAULT_CAPACITY));

    int fd = open("/dev/zero", O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open /dev/zero: %s\n", strerror(errno));
        return NULL;
    }

    void *addr = mmap(NULL, padded_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
    close(fd);

    if (addr == MAP_FAILED) {
        fprintf(stderr, "Failed to map memory: %s\n", strerror(errno));
        return NULL;
    }
    
    madvise(addr, sizeof(Region), MADV_WILLNEED);

    Region *r = addr;
    r->next = NULL;
    r->count = 0;
    return r;
}

void free_region(Region *r)
{
    munmap(r, sizeof(Region));
}

void *arena_alloc(Arena *a, size_t size_bytes)
{
    size_t size = (size_bytes + sizeof(uintptr_t) - 1)/sizeof(uintptr_t);

    if (a->end == NULL) {
        ARENA_ASSERT(a->begin == NULL);
        size_t capacity = REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        a->end = new_region(capacity);
        a->begin = a->end;
        a->new_region_calls++;
    }

    while (a->end->count + size > a->end->capacity && a->end->next != NULL) {
        a->existing_region_skipped++;
        a->end = a->end->next;
    }

    if (a->end->count + size > a->end->capacity) {
        a->allocation_exceeded++;
        ARENA_ASSERT(a->end->next == NULL);
        size_t capacity = REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        a->end->next = new_region(capacity);
        a->end = a->end->next;
        a->new_region_calls++;
    }

    void *result = &a->end->data[a->end->count];
    a->end->count += size;
    return result;
}

void arena_reset(Arena *a)
{
    for (Region *r = a->begin; r != NULL; r = r->next) {
        r->count = 0;
    }
    a->end = a->begin;

    a->new_region_calls = 0;
    a->existing_region_skipped = 0;
    a->allocation_exceeded = 0;
}

void arena_free(Arena *a)
{
    Region *r = a->begin;
    while (r) {
        Region *r0 = r;
        r = r->next;
        free_region(r0);
    }
    a->begin = NULL;
    a->end = NULL;
}
#endif // ARENA_IMPLEMENTATION
