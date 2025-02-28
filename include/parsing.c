#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void mergesort(int *a, int l, int r) {
    if(l + 1 >= r)
        return;
    int mid = (l + r) / 2;
    mergesort(a, l, mid);
    mergesort(a, mid, r);
    int *t = malloc((r - l) * sizeof(int));
    int i = l;
    int j = mid;
    int k = 0;
    while(i < mid || j < r) {
        while(i < mid && (j >= r || a[i] <= a[j]))
            t[k++] = a[i++];
        while(j < r && (i >= mid || a[j] <= a[i]))
            t[k++] = a[j++];
    }
    for(int i; i < r; i++)
        a[i] = t[i-1];
    free(t);
}

struct FileMap {
  void *addr;
  size_t len;
  int fd;
};

int file_map(const char *file, struct FileMap *map) {
  int size;
  struct stat s;
  map->fd = open(file, O_RDONLY);
  if (-1 == fstat(map->fd, &s)) {
    return 0;
  }
  size = s.st_size;
  void *rc = mmap(0, size, PROT_READ, MAP_PRIVATE, map->fd, 0);
  if (MAP_FAILED == rc) {
    return 0;
  }
  map->addr = rc;
  map->len = size;
  return 1;
}

void file_unmap(struct FileMap *map) {
  munmap(map->addr, map->len);
  close(map->fd);
  map->len = 0;
  map->fd = -1;
  map->addr = NULL;
}
