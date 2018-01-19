/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <iostream>
#include <chrono>
using namespace std;

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "image.h" // array-based implementation
#include "perimeter.h" // quad-tree-based implementation

extern int dealwithargs(int argc, char* argv[]);

const int size = 4096;
const int center_x = 0;
const int center_y = 0;

int main(int argc, char *argv[])
{
    int perimeter_method_1;
    int perimeter_method_2;
    int level;

    level = dealwithargs(argc, argv);

    cout << "============================================================" << endl;
    cout << "= Olden perimeter benchmark feasibility study! =" << endl;
    cout << "============================================================" << endl;

    /*
     * compute perimeter of the circular Ring with quad-tree-based implementation
     */

    QuadTree tree;
    int leavesCount;

    cout << "[1] Build image-based quad tree" << endl;

    auto start_time = chrono::high_resolution_clock::now();
    
    tree = MakeTree(size, center_x, center_y, 0, NumNodes-1, NULL, southeast, level);
    
    auto end_time = chrono::high_resolution_clock::now();
    auto treeBuildTime = end_time - start_time;
    
    cout << "[2] Compute the perimeter of the image" << endl;
    
    start_time = chrono::high_resolution_clock::now();
    
    perimeter_method_1 = perimeter(tree, size);
 
    end_time = chrono::high_resolution_clock::now();
    auto perimeterComputeTime_1 = end_time - start_time;
   
    cout << "[3] Count number of quad-tree elements" << endl;
    
    start_time = chrono::high_resolution_clock::now();
    
    leavesCount = CountTree(tree);
    cout << "Number of quad-tree leaves: " << leavesCount << endl;

    end_time = chrono::high_resolution_clock::now();
    auto leavesCountTime = end_time - start_time;

    cout << "Perimeter with " << level << " levels on " << NumNodes << " processors" << endl;

    /*
     * compute perimeter of the circular Ring with array-based implementation
     */
    
    int elementSize = 2*size / pow(2, level+1);
    const int r = 1024; // r^2 = 1048576
    const int R = 2048; // R^2 = 4194304

    Grid grid(2*size/elementSize, elementSize); // computational grid 
    
    Image* image = new RingImage(r, R); // image to map on the computational grid
   
    cout << "[4] Map ring image on the computational grid" << endl;

    start_time = chrono::high_resolution_clock::now();
    
    grid.mapImage(*image);
    image->map(&grid);
 
    end_time = chrono::high_resolution_clock::now();
    auto imageMappingTime = end_time - start_time;
   
    cout << "[5] Print mapped image" << endl;

    start_time = chrono::high_resolution_clock::now();
    
//    image->print();
    cout << endl;
 
    end_time = chrono::high_resolution_clock::now();
    auto imagePrintingTime = end_time - start_time;

    cout << "[6] Compute the perimeter of the image with computational grid" << endl;
    
    start_time = chrono::high_resolution_clock::now();
    
    perimeter_method_2 = image->perimeter();

    end_time = chrono::high_resolution_clock::now();
    auto perimeterComputeTime_2 = end_time - start_time;
 
    /*
     * print the results of the experiment
     */

    cout << "======================================" << endl;
    cout << "= Results (running times in seconds) =" << endl;
    cout << "======================================" << endl;
    cout << "perimeter (quad tree): " << perimeter_method_1 << endl;
    cout << "perimeter (array grid): " << perimeter_method_2 << endl;

    cout << "\tquad tree build time: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(treeBuildTime).count()/1000000 << " seconds.\n";

    cout << "\tquad tree perimeter comp time: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(perimeterComputeTime_1).count()/1000000 << " seconds.\n";

    cout << "\tquad tree total time: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(treeBuildTime + perimeterComputeTime_1).count()/1000000 << " seconds.\n";

    cout << "\tcomp grid image mapping time: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(imageMappingTime).count()/1000000 << " seconds.\n";

    cout << "\tcomp grid image printing time: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(imagePrintingTime).count()/1000000 << " seconds.\n";

    cout << "\tgrid perimeter comp time: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(perimeterComputeTime_2).count()/1000000 << " seconds.\n";

    cout << "\tarray grid total time: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(imageMappingTime + perimeterComputeTime_2).count()/1000000 << " seconds.\n";

    exit(EXIT_SUCCESS);
}
