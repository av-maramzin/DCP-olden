#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cassert>

#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace std;

#include "containers.h"

int main(int argc, char *argv[]) {
    
    cout << "============================================================" << endl;
    cout << "= Olden bisort (bitonic sort) benchmark feasibility study! =" << endl;
    cout << "============================================================" << endl;

    cout << "[1] Generate random olden bisort tree" << endl;
    
    TreeContainer pointerTree(argc, argv);

//    cout << endl;
//    cout << "Generated tree:" << endl;
//    pointerTree.print();
//    cout << endl;

    cout << "[2] Map tree onto array and vector" << endl;
    // initialize array and vector with the contents of the tree
    VectorContainer sequentialVector;
    ArrayContainer sequentialArray(pointerTree.getNumNodes());
    
    pointerTree.linearize_tree(sequentialArray);
    pointerTree.linearize_tree(sequentialVector);

    // initial validity checks
    size_t i;
    for (i = 0; i < pointerTree.getNumNodes(); i++) {
        if (sequentialVector[i] != sequentialArray[i]) {
            exit(EXIT_FAILURE);
        }
    }

//    cout << "Sequential Vector initialized to:" << endl;
//    sequentialVector.print();
//    cout << endl;
    
//    cout << "Sequential Array initialized to:" << endl;
//    sequentialArray.print();
//    cout << endl;



    cout << "[3] Start running time measuring experiment" << endl;
    
    std::srand(std::time(0)); // use current time as a seed for random generator

// original Olden bitonic sorting
    auto start_time = chrono::high_resolution_clock::now();
    
    pointerTree.sort_sequantial(0);
    
    auto end_time = chrono::high_resolution_clock::now();
    auto treeSortTime = end_time - start_time;

// original std::vector sorting
    start_time = chrono::high_resolution_clock::now();
    
    sequentialVector.sort_sequantial();
    
    end_time = chrono::high_resolution_clock::now();
    auto vectorSortTime = end_time - start_time;

// original std::vector sorting
    start_time = chrono::high_resolution_clock::now();
    
    sequentialArray.sort_sequantial();
    
    end_time = chrono::high_resolution_clock::now();
    auto sequentialArraySortTime = end_time - start_time;

// original std::vector sorting
    start_time = chrono::high_resolution_clock::now();
    
    sequentialArray.sort_parallel();
    
    end_time = chrono::high_resolution_clock::now();
    auto parallelArraySortTime = end_time - start_time;

    cout << "======================================" << endl;
    cout << "= Results (running times in seconds) =" << endl;
    cout << "======================================" << endl;
    cout << "\tload size is 2^" << argv[1] << " = " << pointerTree.getNumNodes() << endl;

    cout << "\treference (Olden original bisort): ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(treeSortTime).count()/1000000 << " seconds.\n";

    cout << "\tstd::sort(vector.begin(), vector.end()): ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(vectorSortTime).count()/1000000 << " seconds.\n";

    cout << "\tarray sequential bitonic sort: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(sequentialArraySortTime).count()/1000000 << " seconds.\n";

    cout << "\tarray parallel bitonic sort: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(parallelArraySortTime).count()/1000000 << " seconds.\n";

    return 0;
}
