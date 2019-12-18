#ifndef SEQUENTIAL_CONTAINER
#define SEQUENTIAL_CONTAINER

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class SequentialContainer {

    public:
        SequentialContainer() {}
        virtual ~SequentialContainer() {}
        
        virtual void add(int elem) = 0;
        virtual void remove() = 0;
        virtual void sort_sequantial() = 0;
        virtual void sort_parallel() = 0;
        virtual void print() = 0;
        virtual void clear() = 0;
};

class VectorContainer : public SequentialContainer {

    public:
        VectorContainer() {}
        
        void add(int elem) { _vector.push_back(elem); }
        void remove() { _vector.pop_back(); }
        void sort_sequantial() {
            std::sort(_vector.begin(), _vector.end(), std::greater<int>());
        }
        void sort_parallel() { std::cerr << "VectorContainer()::sort_parallel is not supported!"; }
        void clear() { _vector.clear(); }

        int& operator[](size_t pos) {
            return _vector[pos];
        }

        void print() {
            std::cout << std::endl;
            for(int i = 0; i < _vector.size(); i++) {
                std::cout << _vector[i] << " ";
            }
            std::cout << std::endl;
        }

    private:
        std::vector<int> _vector;
};

#include "bitonic_sort.h"

class ArrayContainer : public SequentialContainer {

    public:
        ArrayContainer(int arraySize) 
            : capacity(arraySize), size(0), array{new int[arraySize]} {}

        ~ArrayContainer() {
            delete [] array;
        }

        void add(int elem) { array[size++] = elem; }
        void remove() {
            if (size > 0) size--;
        };
        
        void sort_sequantial() {
            bitonic_merge_sort_sequential(array, size);
        }
        
        void sort_parallel() {
            bitonic_merge_sort_parallel(array, size);
        }

        int& operator[](size_t pos) {
            return array[pos];
        }

        void print() {
            std::cout << std::endl;
            for(int i = 0; i < size; i++) {
                std::cout << array[i] << " ";
            }
            std::cout << std::endl;
        }

        void clear() { size = 0; };

        bool is_empty() { return (size == 0); }

    private:
        int size;
        int capacity;
        int *array;
};

#include "olden_bitonic_sort.h"

class TreeContainer {
    
    public:
        TreeContainer(int argc, char *argv[]);
        ~TreeContainer();
        
        void sort_sequantial(int direction) {
            sval = Bisort( root, sval, direction);
        }

        void linearize_tree(SequentialContainer& seqCont) {
            seqCont.clear();
            fill(root, seqCont);
        }

        void print() {
            std::cout << std::endl;
            InOrder(root);
            std::cout << std::endl;
        }

        int getNumNodes() {
            return n;
        }
    
    private:
        void fill(HANDLE *h, SequentialContainer& seqCont);

        HANDLE *root;
        int sval;
        int n;
};

#endif // #ifndef SEQUENTIAL_CONTAINER
