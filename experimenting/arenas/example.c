#include <assert.h>
#include <string.h>

#define ARENA_IMPLEMENTATION
#include "../../include/arena.h"

static Arena default_arena = {0};
static Arena *context_arena = &default_arena;

static void *context_alloc(size_t size)
{
    assert(context_arena);
    return arena_alloc(context_arena, size);
}

static void *context_realloc(void *oldp, size_t oldsz, size_t newsz)
{
    if (newsz <= oldsz) return oldp;
    return memcpy(context_alloc(newsz), oldp, oldsz);
}
void region_append(Region *region, void *data)                                                 
{
    while ((region)->count >= region->capacity) {                                
        size_t new_capacity = region->capacity*2;                         
        if (new_capacity == 0) {                                        
            new_capacity = 8192;                               
        }                                                               
                                                                        
        region->data = context_realloc((void*)region->data, region->capacity*sizeof(region->data[0]), new_capacity*sizeof(region->data[0]));  
        region->capacity = new_capacity;                                  
    }                                                                   
                                                                        
    region->data[region->count++] = data;                                
}

int main()
{
    arena_reset(context_arena);
    region
}
