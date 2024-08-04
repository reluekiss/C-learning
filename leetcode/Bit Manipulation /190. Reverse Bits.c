/* Given a 32-bit unsigned integer n, reverse the bits of the binary
   representation of n and return the result.

Example 1:
  Input: n = 00000000000000000000000000010101
  Output:    2818572288 (10101000000000000000000000000000)

  Explanation: Reversing 00000000000000000000000000010101, which represents the
               unsigned integer 21, gives us 10101000000000000000000000000000
               which represents the unsigned integer 2818572288. */

#include <stdint.h>

uint32_t reverseBits(uint32_t n) {
    int ans;
    for (int i = 0; i < 32; i++) {
        uint32_t bit = (n >> i) & 1;
        ans += (bit << (31 - i));
    }
    return ans;    
}
