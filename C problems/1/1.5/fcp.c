#include <stdio.h> 
 
/* copy input to output; 1st version  */ 
int IO1() 
{ 
    int c; 

    c = getchar(); 
    while (c != EOF) { 
        putchar(c); 
        c = getchar(); 
    } 
}

int IO2()
{
    int c;
    
    while ((c = getchar()) != EOF)
        putchar(c);

    printf("%s\n", EOF);
}

int main()
{
    IO2();
}
