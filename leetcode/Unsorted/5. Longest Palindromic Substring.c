/*
5. Longest Palindromic Substring

Given a string s, find the longest palindromic substring in s. You may assume that the maximum length of s is 1000.

Example:
Input: "babad"

Output: "bab"

Note: "aba" is also a valid answer.



Example:
Input: "cbbd"

Output: "bb"
*/
#include <stdio.h>
#include <string.h>

/*char strrev(char* s) {
    char str[100] = "string";
    int len = strlen(str);
    for (int i = 0, j = len - 1; i <= j; i++, j--) {
        char c = str[i];
        str[i] = str[j];
        str[j] = c;
    }
    return *str;
}

char lps(char* s) {
    char tmp[] = {};
    char max = 0;
    char current;
    for(int i = 0, j = 0; i < strlen(s); i++) {
        tmp[i] = s[i];
        int len = strlen(tmp);
        if(strrev(tmp) == *tmp && len > max) {
            max = len;
            current = tmp;
        }

    }
    return *current;
}

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

int manacher_odd(char* s) {
    int n = strlen(s);
    int p[n + 2];
    int l = 1, r = 1;
    for(int i = 1; i <= n; i++) {
        p[i] = max(0, min(r - i, p[l + (r - i)]));
        while(s[i - p[i]] == s[i + p[i]]) {
            p[i]++;
        }
        if(i + p[i] > r) {
            l = i - p[i], r = i + p[i];
        }
    }
    return (begin(p) + 1, end(p) - 1);
}*/

char text[100]; 
int min(int a, int b) { 
    int res = a; 
    if(b < a) 
        res = b; 
    return res; 
} 

void findLongestPalindromicString() { 
    int N = strlen(text); 
    if(N == 0) 
        return; 
    N = 2*N + 1;
    int L[N];
    L[0] = 0; 
    L[1] = 1; 
    int C = 1;
    int R = 2;
    int i = 0;
    int iMirror;
    int maxLPSLength = 0; 
    int maxLPSCenterPosition = 0; 
    int start = -1; 
    int end = -1; 
    int diff = -1; 
    
    for (i = 2; i < N; i++) 
    { 
        iMirror = 2*C-i; 
        L[i] = 0; 
        diff = R - i; 
        if(diff > 0) 
            L[i] = min(L[iMirror], diff); 

        while ( ((i + L[i]) < N && (i - L[i]) > 0) && 
            ( ((i + L[i] + 1) % 2 == 0) || 
            (text[(i + L[i] + 1)/2] == text[(i - L[i] - 1)/2] ))) 
        { 
            L[i]++; 
        } 

        if(L[i] > maxLPSLength) { 
            maxLPSLength = L[i]; 
            maxLPSCenterPosition = i; 
        } 
        if (i + L[i] > R) { 
            C = i; 
            R = i + L[i]; 
        } 
    } 
    start = (maxLPSCenterPosition - maxLPSLength)/2; 
    end = start + maxLPSLength - 1; 
    for(i=start; i<=end; i++) { 
        printf("%c", text[i]);
    }
} 

int main(int argc, char *argv[]) 
{ 

    strcpy(text, "babcbabcbaccba"); 
    findLongestPalindromicString();
    return 0;
}
