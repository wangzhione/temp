#include <stdlib.h>

// 3 1 2
// 

void sort_insertion(int a[], int len) {

    // for (int i = 1; i < len; ++i) {
    //     int j = i;
    //     int anchor = a[j];
    //     while (--j >= 0 && a[j] > anchor) {
    //         a[j+1] = a[j];
    //     }
    //     a[j+1] = anchor;
    // }

    // for (int i = 1; i < len; i++) {
    //     int j = i;
    //     int anchor = a[j];
    //     while (j > 0 && anchor < a[j-1]) {
    //         a[j] = a[j-1];
    //         j--;
    //     }
    //     a[j] = anchor;
    // }

    for (int i = 1; i < len; i++) {
        int j = i;
        int anchor = a[j];
        while (j >= 1 && anchor < a[j-1]) {
            a[j] = a[j-1];
            j--;
        }
        a[j] = anchor;
    }
}

void sort_insertion_upgrade(int a[], int len) {
    for (int i = 1; i < len; i++) {
        int j = i-1;
        int anchor = a[i];
        if (anchor >= a[j]) {
            continue;
        }
        do {
            a[j+1] = a[j];
        } while(--j >= 0 && anchor < a[j]);
        a[j+1] = anchor;
    }
}
