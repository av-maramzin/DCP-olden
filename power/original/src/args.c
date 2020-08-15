/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <stdlib.h>

int feeders_num = 11; //#define NUM_FEEDERS 8
int laterals_num = 21; //#define LATERALS_PER_FEEDER 21
int branches_num = 6; //#define BRANCHES_PER_LATERAL 6
int leaves_num = 12; //#define LEAVES_PER_BRANCH 12

int dealwithargs(int argc, char *argv[])
{
    if (argc > 1)
        feeders_num = atoi(argv[1]);
    
    if (argc > 2)
        laterals_num = atoi(argv[2]);

    if (argc > 3)
        branches_num = atoi(argv[3]);

    if (argc > 4)
        leaves_num = atoi(argv[4]);

    return 0;
}

//
