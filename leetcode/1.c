#include <stdio.h>

int twosum(int arga[], int argc){ 
    int i = 0;
    size_t length = sizeof(arga)/sizeof(arga[0]);
    for(i = 0; i < length; i++){
        for(int j = i; j >= 0; j--){
            if(arga[i] == arga[j]){
                break;
            }
            int sum = arga[i] + arga[j];
            if(sum == argc){
                printf("[%d, %d]\n", i, j);
                return 1;
            }
        }
    }
    return 0;
}

int main(){
    int nums[] = {3,2,4};
    int target=6;

    twosum(nums, target);
}
