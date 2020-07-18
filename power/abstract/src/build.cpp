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
    
    feeders.grow(reduce_width);
    feeders.set_impl_type(Reduce_Feeder::ImplType::parallel);
}

Root::~Root() {} 

// Reduce_Feeder

Feeder::Feeder(Reduce_Feeder::ElementInfo& info)
    : Reduce_Feeder::Element(info) {}

Feeder::~Feeder() {}

void Feeder::grow()
{
    size_t fold_depth = LATERALS_PER_FEEDER;
    fold_lateral.grow(fold_depth-1);
}

// Fold_Lateral

Lateral::Lateral(Fold_Lateral::ElementInfo& info)
    : Fold_Lateral::Element(info) {}

Lateral::~Lateral() {}

void Lateral::grow()
{
    R = 1/300000.0;
    X = 0.000001;
    alpha = 0.0;
    beta = 0.0;

    size_t fold_depth = BRANCHES_PER_LATERAL;
    fold_branch.grow(fold_depth-1);
}

// Fold_Branch

Branch::Branch(Fold_Branch::ElementInfo& info)
    : Fold_Branch::Element(info) {}

Branch::~Branch() {}

void Branch::grow()
{
    R = 0.0001;
    X = 0.00002;
    alpha = 0.0;
    beta = 0.0;

    size_t reduce_width = LEAVES_PER_BRANCH;
    leaves.grow(reduce_width);
}

Leaf::Leaf(Reduce_Leaf::ElementInfo& info)
    : Reduce_Leaf::Element(info) {}

Leaf::~Leaf() {}

void Leaf::grow() {
    D.P = 1.0;
    D.Q = 1.0;
}

//
