#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

struct FileMap {
    void *addr;
    size_t len;
    int fd;
};

static inline int file_map(const char *file, struct FileMap *map) {
    struct stat s;
    map->fd = open(file, O_RDONLY);
    if (-1 == fstat(map->fd, &s)) {
        return 0;
    }
    size_t size = s.st_size;
    void *rc = mmap(0, size, PROT_READ, MAP_PRIVATE, map->fd, 0);
    if (MAP_FAILED == rc) {
        return 0;
    }
    map->addr = rc;
    map->len = size;
    return 1;
}

static inline void file_unmap(struct FileMap *map) {
    munmap(map->addr, map->len);
    close(map->fd);
    map->len = 0;
    map->fd = -1;
    map->addr = NULL;
}
