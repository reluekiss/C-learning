/*
1. Two Sum

Given an array of integers, return indices of the two numbers such that they add up to a specific target.

You may assume that each input would have exactly one solution, and you may not use the same element twice.


Example:
Given nums = [2, 7, 11, 15], target = 9,

Because nums[0] + nums[1] = 2 + 7 = 9,
return [0, 1].
*/
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
