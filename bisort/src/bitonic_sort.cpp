
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cassert>

#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
using namespace std;

void bitonic_merge_sort_sequential(int array[], int n);
void bitonic_merge_sort_parallel(int array[], int n);

void bitonic_merge(int array[], int n);
void bitonic_sort(int array[], int n);

void compare_and_swap(int &a, int &b);

static const int range = 100;

int main(int argc, char *argv[]) {
    int power;
    int size;
    int* array_parallel;
    int* array_sequential;
    
    // standard library containers
    vector<int> vec;

    if (argc < 2) {
        cout << "Usage: ./bitonic_sort n\n\t" 
            << "where n determines the number of elements (2^n)" << endl; 
        return 1;
    } else {
        power = atoi(argv[1]);
    }

    size = pow(2, power);
    array_sequential = new int[size];
    array_parallel = new int[size];

    std::srand(std::time(0)); // use current time as a seed for random generator

    for (int i = 0; i < size; i++) {
        array_parallel[i] = std::rand() % range;
        array_sequential[i] = array_parallel[i];
        vec.push_back(array_parallel[i]);
    }
    
    cout << endl;
    cout << "Testing performance of sorting algorithms on the random array of " 
        << size << " elements!" << endl;

    auto start_time = chrono::high_resolution_clock::now();
    
    sort(vec.begin(), vec.end());
    
    auto end_time = chrono::high_resolution_clock::now();
    auto time = end_time - start_time;
    
    cout << "Running times:" << endl;
    cout << "\tstd::sort(vector): ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(time).count()/1000000 << " seconds.\n";

    start_time = chrono::high_resolution_clock::now();
 
    bitonic_merge_sort_sequential(array_sequential, size);

    end_time = chrono::high_resolution_clock::now();
    time = end_time - start_time;
    
    cout << "\tbitonic merge sort (sequantial): ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(time).count()/1000000 << " seconds.\n";

    start_time = chrono::high_resolution_clock::now();
 
    bitonic_merge_sort_parallel(array_parallel, size);

    end_time = chrono::high_resolution_clock::now();
    time = end_time - start_time;
    
    cout << "\tbitonic merge sort (parallel): ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(time).count()/1000000 << " seconds.\n";
    cout << endl;

    delete [] array_sequential; 
    delete [] array_parallel;

    return 0;
}

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
