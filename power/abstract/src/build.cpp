/* For copyright information, see olden_v1.0/COPYRIGHT */

/* build.c
 *
 * By:  Martin C. Carlisle
 * 6/15/94
 * builds the tree for the Power Pricing problem
 *
 * based on code by:  Steve Lumetta, Sherry Li, and Ismail Khalil
 * University of California at Berkeley
 */

#include "power.h"

Root::Root()
    : feeders(NUM_FEEDERS)
{
    theta_R = 0.8;
    theta_I = 0.16;
   
    for (int i = 0; i < NUM_FEEDERS; i++) {
        feeders[i].grow(LATERALS_PER_FEEDER);
    }
}

Root::~Root() 
{
    for(int i=0; i<NUM_FEEDERS; i++) {
        feeders[i].shrink(0);
    }

    feeders.clear();
}

Lateral::Lateral()
{
    R = 1/300000.0;
    X = 0.000001;
    alpha = 0.0;
    beta = 0.0;

    branch_fold.grow(BRANCHES_PER_LATERAL);
}

Lateral::~Lateral()
{
    branch_fold.shrink(0);
}

Branch::Branch()
    : leaves(LEAVES_PER_BRANCH)
{
    R = 0.0001;
    X = 0.00002;
    alpha = 0.0;
    beta = 0.0;
}

Leaf::Leaf() {
    D.P = 1.0;
    D.Q = 1.0;
}
