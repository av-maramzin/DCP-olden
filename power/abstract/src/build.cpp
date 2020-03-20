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

Root* build_tree()
{
    int i;
    Root* t;
    Lateral* l;

    t = (Root*) malloc(sizeof(*t));

    for (i = 0; i < NUM_FEEDERS; i++) {
        /* Insert future here, split into two loops */
        l = build_lateral(i*LATERALS_PER_FEEDER, LATERALS_PER_FEEDER);
        t->feeders[i]=l;
    }
    
    t->theta_R = 0.8;
    t->theta_I = 0.16;
    
    return t;
}

Root::Root()
{
    int i;
    Root* t;
    Lateral* l;

    t = (Root*) malloc(sizeof(*t));

    for (i = 0; i < NUM_FEEDERS; i++) {
        /* Insert future here, split into two loops */
        l = build_lateral(i*LATERALS_PER_FEEDER, LATERALS_PER_FEEDER);
        t->feeders[i]=l;
    }
    
    t->theta_R = 0.8;
    t->theta_I = 0.16;
    
    return t;
}

Lateral::Lateral()
{
    R = 1/300000.0;
    X = 0.000001;
    alpha = 0.0;
    beta = 0.0;

    branch_fold.grow(BRANCHES_PER_LATERAL);
}

Branch::Branch()
{
    R = 0.0001;
    X = 0.00002;
    alpha = 0.0;
    beta = 0.0;

    for (i = 0; i < LEAVES_PER_BRANCH; i++) {
        leaves[i] = new Leaf();
    }
}

Leaf::Leaf() {
    D.P = 1.0;
    D.Q = 1.0;
}
