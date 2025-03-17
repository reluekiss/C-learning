#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>

#define BUCKET_SIZE 8             // Each bucket holds 8 slots.
#define INITIAL_CAPACITY 64       // Initially active slots (must be a multiple of BUCKET_SIZE).
#define MAX_CAPACITY (1 << 20)    // Reserve space for up to 1M slots.
#define LOAD_FACTOR_UPPER 0.75    // Trigger growth when a bucket is full.
#define NOPS 1000000              // Number of stress-test operations.

typedef struct {
    uint32_t current_capacity;  // Active number of slots in use.
    uint32_t count;             // Current number of stored items.
    uint32_t numBuckets;        // Fixed number of buckets (set at creation).
    // Arrays are allocated for MAX_CAPACITY slots.
    uint32_t *keys;             // Stored keys.
    uint32_t *values;           // Stored values.
    uint8_t  *bitmap;           // One byte per bucket (each bit indicates slot occupancy).
} dtable_t;

static inline uint32_t hash_key(uint32_t key) {
    key ^= key >> 16;
    key *= 0x85ebca6b;
    key ^= key >> 13;
    key *= 0xc2b2ae35;
    key ^= key >> 16;
    return key;
}

static inline void *xmap(size_t size) {
    void *p = mmap(NULL, size, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    return (p == MAP_FAILED) ? NULL : p;
}

// Reserves MAX_CAPACITY but initially uses INITIAL_CAPACITY.
dtable_t* dt_create(void) {
    if (INITIAL_CAPACITY % BUCKET_SIZE != 0)
        return NULL;
    dtable_t *dt = xmap(sizeof(dtable_t));
    if (!dt) return NULL;
    dt->current_capacity = INITIAL_CAPACITY;
    dt->count = 0;
    dt->numBuckets = INITIAL_CAPACITY / BUCKET_SIZE; // Fixed bucket count.
    
    dt->keys = xmap(MAX_CAPACITY * sizeof(uint32_t));
    dt->values = xmap(MAX_CAPACITY * sizeof(uint32_t));
    // Reserve bitmap for MAX_CAPACITY/BUCKET_SIZE buckets.
    dt->bitmap = xmap((MAX_CAPACITY / BUCKET_SIZE) * sizeof(uint8_t));
    if (!dt->keys || !dt->values || !dt->bitmap) {
        if(dt->keys) munmap(dt->keys, MAX_CAPACITY * sizeof(uint32_t));
        if(dt->values) munmap(dt->values, MAX_CAPACITY * sizeof(uint32_t));
        if(dt->bitmap) munmap(dt->bitmap, (MAX_CAPACITY / BUCKET_SIZE) * sizeof(uint8_t));
        munmap(dt, sizeof(dtable_t));
        return NULL;
    }
    // Memory from MAP_ANONYMOUS is already zeroed.
    return dt;
}

void dt_destroy(dtable_t *dt) {
    if (!dt) return;
    munmap(dt->keys, MAX_CAPACITY * sizeof(uint32_t));
    munmap(dt->values, MAX_CAPACITY * sizeof(uint32_t));
    munmap(dt->bitmap, (MAX_CAPACITY / BUCKET_SIZE) * sizeof(uint8_t));
    munmap(dt, sizeof(dtable_t));
}

static inline size_t dt_active_memory_usage(dtable_t *dt) {
    size_t keys_usage = dt->current_capacity * sizeof(uint32_t);
    size_t values_usage = dt->current_capacity * sizeof(uint32_t);
    size_t bitmap_usage = (dt->current_capacity / BUCKET_SIZE) * sizeof(uint8_t);
    return sizeof(dtable_t) + keys_usage + values_usage + bitmap_usage;
}

// Returns the tiny pointer (offset within bucket) or UINT32_MAX on failure.
uint32_t dt_insert(dtable_t *dt, uint32_t key, uint32_t value) {
    // We use a fixed total bucket count based on MAX_CAPACITY.
    uint32_t totalBuckets = MAX_CAPACITY / BUCKET_SIZE;
    uint32_t bucket = hash_key(key) % totalBuckets;
    // Determine the active number of slots per bucket.
    // Here we use: active_slots = current_capacity * BUCKET_SIZE / INITIAL_CAPACITY,
    // capped at BUCKET_SIZE.
    uint32_t active_slots = (dt->current_capacity * BUCKET_SIZE) / INITIAL_CAPACITY;
    if (active_slots > BUCKET_SIZE)
        active_slots = BUCKET_SIZE;
    
    for (uint32_t i = 0; i < active_slots; i++) {
        uint32_t pos = bucket * BUCKET_SIZE + i;
        if (!(dt->bitmap[bucket] & (1 << i))) {
            dt->bitmap[bucket] |= (1 << i);
            dt->keys[pos] = key;
            dt->values[pos] = value;
            dt->count++;
            return i; // Tiny pointer: offset within bucket.
        }
    }
    // If bucket full in the active region, try to grow if possible.
    if (dt->current_capacity < MAX_CAPACITY) {
        uint32_t new_capacity = dt->current_capacity * 2;
        if (new_capacity > MAX_CAPACITY)
            new_capacity = MAX_CAPACITY;
        dt->current_capacity = new_capacity;
        // No rehashing is required since bucket count remains fixed.
        return dt_insert(dt, key, value);
    }
    return UINT32_MAX; // Table full.
}

int dt_lookup(dtable_t *dt, uint32_t key, uint32_t *value_out) {
    uint32_t totalBuckets = MAX_CAPACITY / BUCKET_SIZE;
    uint32_t bucket = hash_key(key) % totalBuckets;
    uint32_t active_slots = (dt->current_capacity * BUCKET_SIZE) / INITIAL_CAPACITY;
    if (active_slots > BUCKET_SIZE)
        active_slots = BUCKET_SIZE;
    for (uint32_t i = 0; i < active_slots; i++) {
        uint32_t pos = bucket * BUCKET_SIZE + i;
        if (dt->bitmap[bucket] & (1 << i)) {
            if (dt->keys[pos] == key) {
                if (value_out)
                    *value_out = dt->values[pos];
                return 1;
            }
        }
    }
    return 0;
}

int dt_delete(dtable_t *dt, uint32_t key) {
    uint32_t totalBuckets = MAX_CAPACITY / BUCKET_SIZE;
    uint32_t bucket = hash_key(key) % totalBuckets;
    uint32_t active_slots = (dt->current_capacity * BUCKET_SIZE) / INITIAL_CAPACITY;
    if (active_slots > BUCKET_SIZE)
        active_slots = BUCKET_SIZE;
    for (uint32_t i = 0; i < active_slots; i++) {
        uint32_t pos = bucket * BUCKET_SIZE + i;
        if (dt->bitmap[bucket] & (1 << i)) {
            if (dt->keys[pos] == key) {
                dt->bitmap[bucket] &= ~(1 << i);
                dt->keys[pos] = 0;
                dt->values[pos] = 0;
                dt->count--;
                return 1;
            }
        }
    }
    return 0;
}

#ifdef TEST_STRESS
// --- Stress Test ---
#include <assert.h>
#include <sys/resource.h>

int main(void) {
    dtable_t *dt = dt_create();
    if (!dt) {
        perror("dt_create failed");
        return 1;
    }
    srand((unsigned)time(NULL));
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Variables to track memory usage over time.
    uint64_t mem_usage_sum = 0;
    uint32_t mem_usage_count = 0;
    uint32_t mem_usage_min = UINT32_MAX;
    uint32_t mem_usage_max = 0;

    // Variables to track effective overhead per item (in bits).
    // We compare our active memory usage with a standard raw key/value array.
    uint64_t overhead_sum = 0;
    uint32_t overhead_samples = 0;
    uint32_t overhead_min = UINT32_MAX;
    uint32_t overhead_max = 0;

    // Auxiliary array to store keys for deletion/lookup.
    uint32_t *aux = malloc(NOPS * sizeof(uint32_t));
    uint32_t aux_count = 0;

    for (uint32_t op = 0; op < NOPS; op++) {
        // Sample current memory usage.
        size_t current_usage = dt_active_memory_usage(dt);
        mem_usage_sum += current_usage;
        mem_usage_count++;
        if (current_usage < mem_usage_min)
            mem_usage_min = current_usage;
        if (current_usage > mem_usage_max)
            mem_usage_max = current_usage;
        
        // If there are stored items, compute effective overhead.
        // Raw usage for a standard array storing key and value per item:
        //   raw = count * (sizeof(uint32_t) * 2)
        if (dt->count > 0) {
            size_t raw_usage = dt->count * (2 * sizeof(uint32_t));
            int extra_bytes = (current_usage > raw_usage) ? (current_usage - raw_usage) : 0;
            uint32_t overhead_bits = extra_bytes * 8;
            overhead_sum += overhead_bits;
            overhead_samples++;
            if (overhead_bits < overhead_min)
                overhead_min = overhead_bits;
            if (overhead_bits > overhead_max)
                overhead_max = overhead_bits;
        }
        
        int r = rand() % 100;
        if (r < 50) { 
            // 50% insertion.
            uint32_t key = rand();
            uint32_t value = rand();
            uint32_t tp = dt_insert(dt, key, value);
            if (tp != UINT32_MAX)
                aux[aux_count++] = key;
        } else if (r < 80) {
            // 30% lookup.
            if (aux_count > 0) {
                uint32_t idx = rand() % aux_count;
                uint32_t key = aux[idx];
                uint32_t found;
                (void) dt_lookup(dt, key, &found);
            }
        } else {
            // 20% deletion.
            if (aux_count > 0) {
                uint32_t idx = rand() % aux_count;
                uint32_t key = aux[idx];
                if (dt_delete(dt, key)) {
                    aux[idx] = aux[aux_count - 1];
                    aux_count--;
                }
            }
        }
    }
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_usec - start.tv_usec) / 1e6;
    size_t final_usage = dt_active_memory_usage(dt);

    // Print summary at end.
    printf("Stress Test Completed:\n");
    printf("  Operations performed: %u\n", NOPS);
    printf("  Elapsed time: %.4f seconds\n", elapsed);
    printf("  Average time per op: %.6f microseconds\n", elapsed * 1e6 / NOPS);
    printf("  Final active capacity: %u slots\n", dt->current_capacity);
    printf("  Final active memory usage: %zu bytes\n", final_usage);
    printf("  Memory usage over time (active region):\n");
    printf("    Minimum: %u bytes\n", mem_usage_min);
    printf("    Maximum: %u bytes\n", mem_usage_max);
    printf("    Average: %llu bytes\n", (unsigned long long)(mem_usage_sum / mem_usage_count));
    
    if (overhead_samples > 0) {
        printf("  Effective overhead per item (over raw key/value data):\n");
        printf("    Minimum: %u bits/item\n", overhead_min);
        printf("    Maximum: %u bits/item\n", overhead_max);
        printf("    Average: %llu bits/item\n", (unsigned long long)(overhead_sum / overhead_samples));
    } else {
        printf("  No items present to measure overhead per item.\n");
    }
    
    free(aux);
    dt_destroy(dt);
    return 0;
}
#endif
