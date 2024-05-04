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
