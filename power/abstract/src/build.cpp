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

// Root

Root::Root()
{
    theta_R = 0.8;
    theta_I = 0.16;
   
    size_t reduce_width = NUM_FEEDERS;
    int reduce_seed = 0;
    
    feeders.grow(reduce_width, reduce_seed);
}

Root::~Root() 
{
    feeders.clear();
}

// Reduce_Feeder

Feeder::Feeder(Reduce_Feeder::ElementInfo& info)
    Reduce_Feeder::Element(info) {}

Feeder::grow(Reduce_Feeder::Seed_t seed)
{
    size_t fold_depth = LATERALS_PER_FEEDER;
    fold_lateral.grow(fold_depth, seed);
}

// Fold_Lateral

Lateral::Lateral(Fold_Lateral::ElementInfo& info)
    Fold_Lateral::Element(info) {}

Lateral::~Lateral() {}

Lateral::grow(Fold_Lateral::Seed_t seed)
{
    R = 1/300000.0;
    X = 0.000001;
    alpha = 0.0;
    beta = 0.0;

    size_t fold_depth = BRANCHES_PER_LATERAL;
    fold_branch.grow(fold_depth, seed);
}

// Fold_Branch

Branch::Branch(Fold_Branch::ElementInfo& info)
    : Fold_Branch::Element(info) {}

Branch::~Branch() {}

Branch::grow(Fold_Branch::Seed_t seed)
{
    R = 0.0001;
    X = 0.00002;
    alpha = 0.0;
    beta = 0.0;

    size_t reduce_width = BRANCHES_PER_LATERAL;
    leaves.grow(reduce_width, seed);
}

Leaf::Leaf(Reduce_Leaf::ElementInfo& info)
    : Reduce_Leaf::Element(info) {}

Leaf::grow() {
    D.P = 1.0;
    D.Q = 1.0;
}

//
