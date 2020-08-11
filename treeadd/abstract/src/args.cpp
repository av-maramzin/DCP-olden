/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <cstdlib>

int NumNodes;

int runs;

int dealwithargs(int argc, char *argv[])
{
    int level;

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
