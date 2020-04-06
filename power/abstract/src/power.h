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

typedef struct demand {
    demand() : P(0.0), Q(0.0) {}
    double P;
    double Q;
} Demand;

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

class Lateral;
struct Lateral::Seed;

using LateralFold = Fold<class Lateral,struct demand,struct Lateral::Seed>;
class Lateral : public LateralFold::Element {

    public:
        
        Lateral();
        ~Lateral();

        struct Seed {
            double theta_R;
            double theta_I;
            double pi_R;
            double pi_I;
        };

        LateralFold::Compute_t compute(LateralFold::Compute_t fold);
        LateralFold::Seed_t inject(LateralFold::Seed_t seed);

    private:
        
        Demand D;
        double alpha;
        double beta;
        double R;
        double X;
        BranchFold branch_fold;
} Lateral;

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
};

using LateralFold = Fold<class Lateral,struct demand,struct Lateral::Seed>;
class Lateral : public LateralFold::Element {

    public:
        
        Lateral();
        ~Lateral();

        struct Seed {
            double theta_R;
            double theta_I;
            double pi_R;
            double pi_I;
        };

        LateralFold::Compute_t compute(LateralFold::Compute_t fold);
        LateralFold::Seed_t inject(LateralFold::Seed_t seed);

    private:
        
        Demand D;
        double alpha;
        double beta;
        double R;
        double X;
        BranchFold branch_fold;
} Lateral;

using BranchFold = Fold<class Branch,struct demand,struct Branch::Seed>;
class Branch : public BranchFold::Element {
    
    public:
        
        Branch() : leaves(LEAVES_PER_BRANCH) {}
        ~Branch();

        struct Seed {            
            double theta_R;
            double theta_I;
            double pi_R;
            double pi_I;
        };
        
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

class Leaf {

    public:

        Leaf();
        ~Leaf() {}

    private:
        
        Demand D;
        double pi_R;
        double pi_I;
};

