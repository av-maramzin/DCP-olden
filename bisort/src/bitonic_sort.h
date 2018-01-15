#ifndef BITONIC_SORT
#define BITONIC_SORT

void bitonic_merge_sort_sequential(int array[], int n);
void bitonic_merge_sort_parallel(int array[], int n);

void bitonic_merge(int array[], int n);
void bitonic_sort(int array[], int n);

void compare_and_swap(int &a, int &b);

static const int range = 100;

#endif // #ifndef BITONIC_SORT
