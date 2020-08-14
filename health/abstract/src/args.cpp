/* For copyright information, see olden_v1.0/COPYRIGHT */

/*****************************************************************
 * args.c:  Handles arguments to command line.                   *
 *          To be used with health.c.                            *
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include "health.h"

bool parallel;
bool balanced;

void dealwithargs(int argc, char* argv[]) { 
  
    if (argc < 4) {
        max_level = 3;
        max_time = 15;
        seed = 4;
    } else {
        max_level = atoi(argv[1]);
        max_time = atol(argv[2]);
        seed = atol(argv[3]);
        
        if (std::atoi(argv[4]) == 1) {
            parallel = true;
        } else if (std::atoi(argv[4]) == 0) {
            parallel = false;
        } else {
            parallel = false;
        }

        if (std::atoi(argv[5]) == 1) {
            balanced = true;
        } else if (std::atoi(argv[5]) == 0) {
            balanced = false;
        } else {
            balanced = false;
        }
    }

    printf("max_level=%d  max_time=%d  seed=%d \n", max_level, (int)max_time, (int)seed);
}

//
