#include <stdio.h>
#include <gmp.h>

void power(mpz_t result, const mpz_t a, const mpz_t n) {
    if (mpz_cmp_ui(n, 0) == 0) {
        mpz_set_ui(result, 1);
    } else {
        mpz_t temp;
        mpz_init(temp);
        power(temp, a, n);
        mpz_mul(result, a, temp);
        mpz_clear(temp);
    }
}

void tower(mpz_t result, const mpz_t a, const mpz_t b) {
    if (mpz_cmp_ui(b, 0) == 0) {
        printf("Error: b == 0\n");
        mpz_set_ui(result, 0); // Set result to 0 in case of error
    } else if (mpz_cmp_ui(b, 1) == 0) {
        mpz_set(result, a);
    } else {
        mpz_t temp;
        mpz_init(temp);
        tower(temp, a, b);
        power(result, a, temp);
        mpz_clear(temp);
    }
}

void arr(mpz_t result, const mpz_t a, const mpz_t s, const mpz_t b);

void arr2(mpz_t result, const mpz_t cnt, const mpz_t a, const mpz_t sp, const mpz_t b) {
    if (mpz_cmp_ui(cnt, 0) == 0) {
        arr(result, a, sp, b);
    } else {
        mpz_t temp;
        mpz_init(temp);
        arr2(temp, cnt, a, sp, b);
        arr(result, a, temp, sp);
        mpz_clear(temp);
    }
}

void arr(mpz_t result, const mpz_t a, const mpz_t s, const mpz_t b) {
    if (mpz_cmp_ui(s, 1) == 0) {
        power(result, a, b);
    } else if (mpz_cmp_ui(s, 2) == 0) {
        tower(result, a, b);
    } else {
        mpz_t temp_b;
        mpz_t temp_s;
        mpz_init(temp_b);
        mpz_init(temp_s);
        mpz_sub_ui(temp_b, b, 1);
        mpz_sub_ui(temp_s, s, 1);
        arr2(result, temp_b, a, temp_s, a);
        mpz_clear(temp_b);
        mpz_clear(temp_s);
    }
}

void size(int n) {
    mpz_t val;
    mpz_init_set_ui(val, 2);
    mpz_t i;
    mpz_init_set_ui(i, 0);
    while (mpz_cmp_ui(i, n) == 0) {
        mpz_t result;
        mpz_init(result);
        arr(result, val, val, i);
        gmp_printf("%Zd\n", result);
        mpz_clear(result);
        mpz_add_ui(i, i, 1);
    }
    mpz_clear(i);
    mpz_clear(val);
}

int main() {
    size(9);
    return 0;
}
