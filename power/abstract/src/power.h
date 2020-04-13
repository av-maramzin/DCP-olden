#ifndef POWER_H
#define POWER_H

/* For copyright information, see olden_v1.0/COPYRIGHT */

/*
 * power.h
 *
 * By:  Martin C. Carlisle
 * 6/15/94
 *
 * Header file for the Power Pricing problem
 * adapted from code by Steve Lumetta, Sherry Li, and Ismail Khalil
 * University of California at Berkeley
 *
 */


#include <math.h>

#define NULL 0

#ifdef SMALL_PROBLEM_SIZE   /* __llvm__ SCALED BACK SETTINGS */
#define NUM_FEEDERS 8
#define LATERALS_PER_FEEDER 16
#define BRANCHES_PER_LATERAL 5
#define LEAVES_PER_BRANCH 10
#else
#define NUM_FEEDERS 11
#define LATERALS_PER_FEEDER 21
#define BRANCHES_PER_LATERAL 6
#define LEAVES_PER_BRANCH 12
#endif

#define F_EPSILON 0.000001
#define G_EPSILON 0.000001
#define H_EPSILON 0.000001
#define ROOT_EPSILON 0.00001

#include "Fold.h"

using namespace abstract;

typedef struct demand {
    demand() : P(0.0), Q(0.0) {}
    double P;
    double Q;
} Demand;

class Leaf {

    public:

        Leaf();
        ~Leaf() {}

    private:
        
        Demand D;
        double pi_R;
        double pi_I;
};

// BranchFold - Fold of Laterals

class Branch;
struct BranchSeed {
    double theta_R;
    double theta_I;
    double pi_R;
    double pi_I;
};

using BranchFold_Element = class Branch;
using BranchFold_Compute = struct demand;
using BranchFold_Seed = struct BranchSeed;
using BranchFold = Fold<BranchFold_Element,
                        BranchFold_Compute,
                        BranchFold_Seed>;

class Branch : public BranchFold::Element {
    
    public:
        
        Branch();
        ~Branch();
        
        BranchFold::Seed_t inject(BranchFold::Seed_t seed);
        BranchFold::Compute_t compute(BranchFold::Compute_t fold);
    
    private:

        Demand D;
        double alpha;
        double beta;
        double R;
        double X;
        //struct leaf* leaves[LEAVES_PER_BRANCH];
        std::vector<Leaf> leaves;
};

// LateralFold - Fold of Laterals

class Lateral;
struct LateralSeed {
    double theta_R;
    double theta_I;
    double pi_R;
    double pi_I;
};

using LateralFold_Element = class Lateral;
using LateralFold_Compute = struct demand;
using LateralFold_Seed = struct LateralSeed;
using LateralFold = Fold<LateralFold_Element,
                         LateralFold_Compute,
                         LateralFold_Seed>;

class Lateral : public LateralFold::Element {

    public:
        
        Lateral();
        ~Lateral();

        LateralFold::Compute_t compute(LateralFold::Compute_t fold);
        LateralFold::Seed_t inject(LateralFold::Seed_t seed);

    private:
        
        Demand D;
        double alpha;
        double beta;
        double R;
        double X;
        BranchFold branch_fold;
};

class Root {
    
    public:

        Root();
        ~Root();

        Demand compute();

    private:
        
        Demand D;
        double theta_R; 
        double theta_I; 
        Demand last;
        double last_theta_R; 
        double last_theta_I;
        std::vector<LateralFold> feeders; // [NUM_FEEDERS];
        Reduce<LateralFold> feeders; // [NUM_FEEDERS];
};

#endif // #ifndef POWER_H
