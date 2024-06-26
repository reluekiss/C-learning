/*
10. Regular Expression Matching

Implement regular expression matching with support for '.' and '*'.

'.' Matches any single character.
'*' Matches zero or more of the preceding element.

The matching should cover the entire input string (not partial).

The function prototype should be:
bool isMatch(const char *s, const char *p)

Some examples:
isMatch("aa","a") → false
isMatch("aa","aa") → true
isMatch("aaa","aa") → false
isMatch("aa", "a*") → true
isMatch("aa", ".*") → true
isMatch("ab", ".*") → true
isMatch("aab", "c*a*b") → true
*/
int regex(char* s, char* p) {
    if (p != s) {
        return 0;
    }
    if (p == "*") {
        return 1;
    }
    return 1;
}

int main() 
{
    char s[] = "aa";
    char p[] = "a";
    
    regex(s, p);
}
