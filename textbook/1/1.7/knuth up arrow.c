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

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

__int128 power(__int128 a, __int128 n)
{
    if (n == 0) {
        return 1;
    } else {
        return a * power(a, n - 1);
    }
}

__int128 tower(__int128 a, __int128 b)
{
    if (b == 0) {
        printf("Error: b == 0\n");
    } else if (b == 1) {
        return a;
    } else {
        return power(a, tower(a, b - 1));
    }
    return 0;
}

__int128 arr(__int128 a, __int128 b, __int128 s);

__int128 arr2(__int128 cnt, __int128 a, __int128 sp, __int128 b)
{
    if (cnt == 0) {
        return arr(a, sp, b);
    } else {
        return arr(a, arr2(cnt - 1, a, sp, b), sp);
    }
}

__int128 arr(__int128 a, __int128 s, __int128 b)
{
    if (s == 1) {
        return power(a, b);
    } else if (s == 2) {
        return tower(a, b);
    } else {
        return arr2(b - 1, a, s - 1, a);
    }
}

void print(__int128 x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x > 9) print(x / 10);
    putchar(x % 10 + '0');
}

__int128 size(int n)
{
    __int128 val = 2;
    for (int i = 0; i < n; i++) {
        val = arr(val, val, i);
        print(val);
        putchar('\n');
    }
    return val;
}

int main() {
    size(9);
    return 0;
}
