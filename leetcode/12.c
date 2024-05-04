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
