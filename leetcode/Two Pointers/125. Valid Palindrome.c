/*
* A phrase is a palindrome if, after converting all uppercase letters into lowercase letters and removing all non-alphanumeric characters, it reads the same forward and backward. Alphanumeric characters include letters and numbers.
* 
* Given a string s, return true if it is a palindrome, or false otherwise.
* 
* Example 1:
* 
* Input: s = "A man, a plan, a canal: Panama"
* Output: true
* Explanation: "amanaplanacanalpanama" is a palindrome.
*
* Example 2:
* 
* Input: s = "race a car"
* Output: false
* Explanation: "raceacar" is not a palindrome.
*
* Example 3:
* 
* Input: s = " "
* Output: true
* Explanation: s is an empty string "" after removing non-alphanumeric characters.
* Since an empty string reads the same forward and backward, it is a palindrome.
*  
* Constraints:
* 
* 1 <= s.length <= 2 * 105
* s consists only of printable ASCII characters.
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool isPalindrome(char* s) {
    int n = strlen(s);
    if (n == 1) return true;

    int left = 0;
    int right = n - 1;

    while (left < right) {
        if (isalnum(s[left]) && isalnum(s[right])) {
            char leftChar = tolower(s[left]);
            char rightChar = tolower(s[right]);

            if (leftChar != rightChar) {
                return false;
            }
        left++;
        right--;
        }
        
        if (!isalnum(s[left])) left++;
        if (!isalnum(s[right])) right--;
    }
    return true;
}

int main()
{
    char s[] = "A man, a plan, a canal: Panama";
    printf("%b", isPalindrome(s));
}
