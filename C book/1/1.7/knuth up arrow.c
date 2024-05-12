// Due to exercise 1.15, simply being to refactor the temperature conversion program I thought I should implement a knuth up arrow function instead. Please excuse the unsigned long longs, with knuth up arrows, the numbers grow really quickly have a look at the tables on wikipedia: https://en.wikipedia.org/wiki/Knuth%27s_up-arrow_notation#Tables_of_values

/** * Knuth's up-arrow notation function
 *
 * Computes a ↑↑ n, where a and b are non-negative integers and n is a non-negative integer.
 *
 * @param a the base
 * @param b the exponent
 * @param n the up-arrow level
 * @return the result of a ↑↑ n
 */

#include <stdio.h>
#include <stdint.h>

unsigned long long power(unsigned long long a, unsigned long long n) {
    if (n == 0) {
        return 1;
    } else {
        return a * power(a, n - 1);
    }
}

unsigned long long tower(unsigned long long a, unsigned long long b) {
    if (b == 0) {
        printf("Error: b == 0\n");
    } else if (b == 1) {
        return a;
    } else {
        return power(a, tower(a, b - 1));
    }
    return 0;
}

unsigned long long arr(unsigned long long a, unsigned long long b, unsigned long long s);

unsigned long long arr2(unsigned long long cnt, unsigned long long a, unsigned long long sp, unsigned long long b) {    if (cnt == 0) {
        return arr(a, sp, b);
    } else {
        return arr(a, arr2(cnt - 1, a, sp, b), sp);
    }
}

unsigned long long arr(unsigned long long a, unsigned long long s, unsigned long long b) {    if (s == 1) {
        return power(a, b);
    } else if (s == 2) {
        return tower(a, b);
    } else {
        return arr2(b - 1, a, s - 1, a);
    }
}


int main() {
    printf("%llu\n", arr(2, 2, 4));
    printf("%llu\n", arr(3, 2, 3));
    return 0;
}
