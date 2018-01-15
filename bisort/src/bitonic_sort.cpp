#include "bitonic_sort.h"

void bitonic_merge_sort_parallel(int array[], int n) {

    if (n > 1) {
        #pragma omp parallel sections
        {
        #pragma omp section
            bitonic_merge_sort_parallel(array, n/2);
        #pragma omp section
            bitonic_merge_sort_parallel(array + n/2, n/2);
        }
        bitonic_merge(array, n);
    }
}

void bitonic_merge_sort_sequential(int array[], int n) {

    if (n > 1) {
        bitonic_merge_sort_sequential(array, n/2);
        bitonic_merge_sort_sequential(array + n/2, n/2);
        bitonic_merge(array, n);
    }
}

/*
 * bitonic_merge():
 * merges two sorted sequences into one sorted sequence
 */
void bitonic_merge(int array[], int n) {
    
    for (int i = 0; i < n/2; i++) {
        compare_and_swap(array[i], array[n-i-1]); 
    }
    
    if (n > 2) {
        bitonic_sort(array, n/2);
        bitonic_sort(array+n/2, n/2);
    }
}

/*
 * bitonic_sort(): sorts bitonic sequences
 */
void bitonic_sort(int array[], int n) {

    int i;
    // half-cleaner: 
    // divides bitonic sequence into 2 bitonic subsequences of equal length;
    // any value in the left subsequence is less than (or equal to) all values 
    // in the right subsequence;
    for (i = 0; i < n/2; i++) {
        compare_and_swap(array[i], array[i+n/2]); 
    }
    
    if (n > 2) {
        bitonic_sort(array, n/2);
        bitonic_sort(array+n/2, n/2);
    } 
}

/*
 * compare_and_swap():
 * smaller value to the left, bigger value to the right
 */
void compare_and_swap(int &a, int &b) {
    int tmp;
   
    if (a > b) {
        tmp = a;
        a = b;
        b = tmp;
    }
}
