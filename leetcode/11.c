#include <stdio.h>
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))


int MaxArea(int* height, int size) {
        int left = 0;
        int right = size - 1;
        int max = 0;
        while(left < right){
            int w = right - left;
            int h = min(height[left], height[right]);
            int area = h * w;
            max = max(max, area);
            if(height[left] < height[right]) left++;
            else if(height[left] > height[right]) right--;
            else {
                left++;
                right--;
            }
        }
    return max;
}

int main()
{
    int height[] = {1,8,6,2,5,4,8,3,7};
    int size = sizeof(height) / sizeof(height[0]);
    printf("%d", MaxArea(height, size));
}
