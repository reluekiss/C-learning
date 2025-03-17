#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define N 32
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

int main(void) {
    uint32_t M[N], inv[N];
    for (int i = 0; i < N; i++) {
        M[i] = (1u << ((i + 7) % N)) ^ (1u << ((i + 18) % N)) ^ (1u << ((i + 3) % N));
        inv[i] = 1u << i;
    }

    for (int i = 0; i < N; i++) {
        int pivot = i;
        while (pivot < N && !(M[pivot] & (1u << i)))
            pivot++;
        if (pivot == N) {
            fprintf(stderr, "Matrix not invertible.\n");
            exit(1);
        }
        if (pivot != i) {
            uint32_t tmp = M[i];
            M[i] = M[pivot];
            M[pivot] = tmp;
            tmp = inv[i];
            inv[i] = inv[pivot];
            inv[pivot] = tmp;
        }
        for (int j = 0; j < N; j++) {
            if (j != i && (M[j] & (1u << i))) {
                M[j] ^= M[i];
                inv[j] ^= inv[i];
            }
        }
    }

    uint32_t y = 0b01110100111110000011001111101110;
    uint32_t x = 0;
    for (int i = 0; i < N; i++) {
        uint32_t prod = inv[i] & y;
        int parity = 0;
        while (prod) {
            parity ^= prod & 1;
            prod >>= 1;
        }
        if (parity)
            x |= 1u << i;
    }

    uint32_t y_computed = ROTR(x, 7) ^ ROTR(x, 18) ^ ROTR(x, 3);
    
    printf("Computed x = 0b%32B\n", x);
    printf("Computed y = 0b%32B\n", y_computed);
    printf("Expected y = 0b%32B\n", y);
    
    if (y_computed == y)
        printf("Verification succeeded.\n");
    else
        printf("Verification failed.\n");
    return 0;
}
