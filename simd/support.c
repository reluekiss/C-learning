#include <stdio.h>

int main() {    if (__builtin_cpu_supports("avx")) {
        printf("AVX supported\n");
    }
    if (__builtin_cpu_supports("avx2")) {
        printf("AVX2 supported\n");
    }
    if (__builtin_cpu_supports("avx512f")) {
        printf("AVX-512 supported\n");
    }
    return 0;
}

