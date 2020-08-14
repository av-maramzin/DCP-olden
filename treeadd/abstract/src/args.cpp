/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <cstdlib>
//#include <string>
//#include <iostream>

int NumNodes;

int runs;

bool parallel;
bool balanced;

int dealwithargs(int argc, char* argv[])
{
    int level;

    if (argc > 5)
        if (std::atoi(argv[5]) == 1) {
            balanced = true;
        } else if (std::atoi(argv[5]) == 0) {
            balanced = false;
        } else {
            balanced = false;
        }
    else
        balanced = false;

    if (argc > 4)
        if (std::atoi(argv[4]) == 1) {
            parallel = true;
        } else if (std::atoi(argv[4]) == 0) {
            parallel = false;
        } else {
            parallel = false;
        }
    else
        parallel = false;

    if (argc > 3)
        runs = std::atoi(argv[3]);
    else
        runs = 100;

    if (argc > 2) 
        NumNodes = std::atoi(argv[2]);
    else 
        NumNodes = 4;

    if (argc > 1)
        level = std::atoi(argv[1]);
    else
        level = 16;

    return level;
}

//
