#include <stdlib.h>

void sort_selection(int a[], int len) {
    if (a == NULL || len <= 1) {
        return;
    } 

    // i 控制循环次数 len-2 - 0 + 1 = len - 1
    for (int i = 0; i < len-1; i++) {
        int idx = i;
        int miv = a[idx];
        for (int j = i+1; j < len; j++) {
            if (miv > a[j]) {
                miv = a[idx = j];
            }
        }
        if (idx != i) {
            a[idx] = a[i];
            a[i] = miv;
        }
    }
}