/* Given two strings s and t of lengths m and n respectively, return the minimum
window substring of s such that every character in t (including duplicates) is 
included in the window. If there is no such substring, return the empty string "".

The testcases will be generated such that the answer is unique.

Example 1:
  Input: s = "ADOBECODEBANC", t = "ABC"
  Output: "BANC"
  Explanation: The minimum window substring "BANC" includes 'A', 'B', and 'C'
  from string t.

Example 2:
  Input: s = "a", t = "a"
  Output: "a"
  Explanation: The entire string s is the minimum window.

Example 3:
  Input: s = "a", t = "aa"
  Output: ""
  Explanation: Both 'a's from t must be included in the window.
  Since the largest window of s only has one 'a', return empty string.
 
Constraints:
  m == s.length
  n == t.length */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

char* minWindow(char* s, char* t){
    int m = strlen(s);
    int n = strlen(t);

    int* alpha = calloc(52, sizeof(int));
    int* alphaCopy = calloc(52 , sizeof(int));
    for(int i = 0; i < n; i++){
        (t[i] >= 'a') ? alpha[t[i] - 'a']++ : alpha[t[i] - 'A' + 26]++;
    }
    
    int min = INT_MAX;
    int l = -1, r;
    int match = 0;
    int a_idx = -1;
    int r_key, l_key;
    for(r = 0; r < m; r++){
        r_key = (s[r] >= 'a') ? r_key = s[r] -'a' : s[r] -'A' + 26;

        if(alpha[r_key] > 0){
            if(l == -1)
                l = r;
            alphaCopy[r_key]++;
            if(alphaCopy[r_key] <= alpha[r_key]){
                match++;                
                if(match == n){
                    l_key = (s[l] >= 'a') ? s[l] -'a' : s[l] -'A' + 26; 
                    while(match == n || alpha[l_key] == 0){   
                        if(alpha[l_key] > 0){
                            alphaCopy[l_key]--; 
                            if(alphaCopy[l_key] < alpha[l_key]){
                                if((r-l+1) < min){
                                    min = r-l+1;
                                    a_idx = l;
                                }
                                match--;                                
                            }
                        }   
                        l++; 
                       if(l < m)
                            l_key = (s[l] >= 'a') ?  s[l] -'a' : s[l] -'A' + 26;
                       else
                          break;
                    }
                }
            }
        }
    }
    if(a_idx == -1)
        return "";
    char* ans = malloc((min + 1) * sizeof(char));
    ans[min] = '\0';
    strncpy(ans, &s[a_idx], min);
    return ans;
}

int main()
{
	char* s = "ADOBECODEBANC";
	char* t = "ABC";

	char* ans = minWindow(s, t);
	printf("%s", ans);
	
	return 0;
}
