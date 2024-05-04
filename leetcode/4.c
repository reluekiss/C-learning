#include <stdio.h>
double findMedianSortedArrays(int* nums1, int* nums2) {
    size_t n1 = sizeof(nums1) / sizeof(int);
    size_t n2 = sizeof(nums2) / sizeof(int);
    
    int temp[n1+n2];
    int x =0;
        for (int i = 0 ; i<n1+n2 ; i++){
            if (i < n1)
               temp[i] = nums1[i];
            else
                temp[i] = nums2[x++];

        }
        for(int i=0;i<n1+n2;i++){
            for(int j=i+1;j<n1+n2;j++){
                if(temp[i]>temp[j]){
                    int t= temp[i];
                    temp[i]=temp[j];
                    temp[j]=t;
                }
            }
        }
        if ((n1+n2) %2 ==0){
            int l = (n1+n2) /2;
            return (temp[l]+temp[l-1])/2.00;
        }
        else {
            return temp[(n1+n2)/2];
        }
}

int main()
{
    int nums1[] = {1,3,4}, nums2[] = {2,6};
    printf("%f", findMedianSortedArrays(nums1, nums2));
    return 0;
}
