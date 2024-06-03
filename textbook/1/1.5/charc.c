#include <stdio.h> 

/* count characters in input; 1st version */ 
int CCIO1() 
{ 
    long nc; 

    nc = 0; 
    while (getchar() != EOF) 
        ++nc; 
    printf("%ld\n", nc); 
} 

int CCIO2()
{
double nc; 

    for (nc = 0; getchar() != EOF; ++nc); 
    printf("%.0f\n", nc); 
    
    return 0;
}

int main()
{
    CCIO2();
    return 0;
}
