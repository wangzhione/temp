#include <iostream>
#include <vector>

void sift_down(std::vector<int> & nums, int k, int end) {
    while (2*k+1 <= end) {
        int j = 2*k + 1;
        if(j+1 < end && nums[j+1] > nums[j]) j++;
        if(nums[j] <= nums[k]) break;
        std::swap(nums[k], nums[j]);
        k = j;
    }
}

void sift_down(std::vector<int> & nums, int parent, int end) {
    int node = nums[parent];

    int child = 2*parent+1;
    while (child <= end) {
        if (child+1 <= end && nums[child+1] > nums[child]) 
            child++;
        if (nums[child] <= node)
            break;

        nums[parent] = nums[child];
        parent = child;
        child = 2*parent+1;
    }
    nums[parent] = node;
}
