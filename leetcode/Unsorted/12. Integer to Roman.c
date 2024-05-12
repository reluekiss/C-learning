/*
12. Integer to Roman

Roman numerals are represented by seven different symbols: I, V, X, L, C, D and M.

Symbol       Value
I             1
V             5
X             10
L             50
C             100
D             500
M             1000

For example, two is written as II in Roman numeral, just two one's added together. Twelve is written as, XII, which is simply X + II. The number twenty seven is written as XXVII, which is XX + V + II.

Roman numerals are usually written largest to smallest from left to right. However, the numeral for four is not IIII. Instead, the number four is written as IV. Because the one is before the five we subtract it making four. The same principle applies to the number nine, which is written as IX. There are six instances where subtraction is used:


	I can be placed before V (5) and X (10) to make 4 and 9. 
	X can be placed before L (50) and C (100) to make 40 and 90. 
	C can be placed before D (500) and M (1000) to make 400 and 900.


Given an integer, convert it to a roman numeral. Input is guaranteed to be within the range from 1 to 3999.

Example 1:

Input: 3
Output: "III"

Example 2:

Input: 4
Output: "IV"

Example 3:

Input: 9
Output: "IX"

Example 4:

Input: 58
Output: "LVIII"
Explanation: L = 50, V = 5, III = 3.


Example 5:

Input: 1994
Output: "MCMXCIV"
Explanation: M = 1000, CM = 900, XC = 90 and IV = 4.
*/
#include <stdio.h>

void itor(int num, char *result) {    
    char ones[10][4] = {"","I","II","III","IV","V","VI","VII","VIII","IX"};
    char tens[10][4] = {"","X","XX","XXX","XL","L","LX","LXX","LXXX","XC"};
    char hrns[10][4] = {"","C","CC","CCC","CD","D","DC","DCC","DCCC","CM"};
    char ths[4][3]={"","M","MM","MMM"};

    sprintf(result, "%s%s%s%s", ths[num/1000], hrns[(num%1000)/100], tens[(num%100)/10], ones[num%10]);
}

int main() {    
    int num = 1589;
    char result[16];
    itor(num, result);
    printf("%s\n", result);
    return 0;
}
