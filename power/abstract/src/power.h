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

// Reduce_Leaf - Reduce of Leaves

class Leaf;
using Reduce_Leaf_ElementType = class Leaf;
using Reduce_Leaf_SeedType = int;
struct Reduce_Leaf_InjectType_struct {
    double pi_R;
    double pi_I;
};
using Reduce_Leaf_InjectType = struct Reduce_Leaf_InjectType_struct;
using Reduce_Leaf = Reduce<Reduce_Leaf_ElementType,
                           Reduce_Leaf_SeedType,
                           Reduce_Leaf_InjectType>;

class Leaf : public Reduce_Leaf::Element {

    public:

        Leaf(Reduce_Leaf::ElementInfo& info);
        ~Leaf() override;
        
        void grow(Reduce_Leaf::Seed_t seed) override;
        
        Reduce_Leaf::Inject_t inject(Reduce_Leaf::Inject_t data) override;

    private:
        
        Demand D;
        double pi_R;
        double pi_I;
};

using Reduce_Leaf_ComputeType = struct demand;
class Leaf_ComputeFunc : public Reduce_Leaf::ComputeFunction<Reduce_Leaf_ComputeType> {
    
    public:

        Reduce_Feeder_ComputeType operator()(Reduce_Leaf::Element_t&) override;
        Reduce_Feeder_ComputeType operator()(const std::vector<Reduce_Leaf_ComputeType>&) override;
};

// Fold_Branch - Foldf of Branches

using Fold_Branch_Element = class Branch;
using Fold_Branch_Seed = int;
struct Fold_Branch_InjectType_struct {
    double theta_R;
    double theta_I;
    double pi_R;
    double pi_I;
};
using Fold_Branch_InjectType = struct Fold_Branch_InjectType_struct;
using Fold_Branch = Fold<Fold_Branch_ElementType,
                         Fold_Branch_SeedType,
                         Fold_Branch_InjectType>;

class Branch : public Fold_Branch::Element {
    
    public:
        
        Branch();
        ~Branch();
        
        void grow(Fold_Branch::Seed_t seed) override;
        Fold_Branch::Inject_t inject(Fold_Branch::Inject_t injected_data) override;
        Fold_Branch::Seed_t spawn_child_seed() override; 

    private:

        Demand D;
        double alpha;
        double beta;
        double R;
        double X;
        Reduce_Leaf leaves;
        //struct leaf* leaves[LEAVES_PER_BRANCH];
        //std::vector<Leaf> leaves;
};

using Fold_Branch_ComputeType = struct demand;
class Branch_ComputeFunc : public Fold_Branch::ComputeFunction<Fold_Branch_ComputeType> {
    
    public:

        Fold_Branch_ComputeType operator()(Fold_Branch::Element_t& element, 
                                           Fold_Branch_ComputeType cumulative) override;
};

// Fold_Lateral - Fold of Laterals

class Lateral;
using Fold_Lateral_ElementType = class Lateral;
using Fold_Lateral_SeedType = int;
struct Fold_Lateral_InjectType_struct {
    double theta_R;
    double theta_I;
    double pi_R;
    double pi_I;
};
using Fold_Lateral_InjectType = struct Fold_Lateral_InjectType_struct;
using Fold_Lateral = Fold<Fold_Lateral_ElementType,
                          Fold_Lateral_SeedType,
                          Fold_Lateral_InjectType>;

class Lateral : public Fold_Lateral::Element {

    public:
        
        Lateral(Fold_Lateral::ElementInfo& info);
        ~Lateral() override;

        void grow(Fold_Lateral::Seed_t seed) override;
        Fold_Lateral::Inject_t inject(Fold_Lateral::Inject_t injected_data) override;
        Fold_Lateral::Seed_t spawn_child_seed() override; 

    private:
        
        Demand D;
        double alpha;
        double beta;
        double R;
        double X;
        Fold_Branch branch_fold;
};

using Fold_Lateral_ComputeType = struct demand;
class Lateral_ComputeFunc : public Fold_Lateral::ComputeFunction<Fold_Lateral_ComputeType> {
    
    public:

        Fold_Lateral_ComputeType operator()(Fold_Lateral::Element_t& element, 
                                            Fold_Lateral_ComputeType cumulative) override;
};

// Reduce_Feeder - Reduce of Feeders

class Feeder;
using Reduce_Feeder_ElementType = class Feeder;
using Reduce_Feeder_SeedType = int;
using Reduce_Feeder = Reduce<Reduce_Feeder_ElementType,
                             Reduce_Feeder_SeedType>;

class Feeder : public Reduce_Feeder::Element {

    public:
        
        Feeder(Reduce_Feeder::ElementInfo& info);
        ~Feeder() override;

        void grow(Reduce_Feeder::Seed_t seed) override;

    private:
        
        Fold_Lateral fold_lateral;
};

using Reduce_Feeder_ComputeType = struct demand;
class Feeder_ComputeFunc : public Reduce_Feeder::ComputeFunction<Reduce_Feeder_ComputeType> {
    
    public:

        Reduce_Feeder_ComputeType operator()(Reduce_Feeder::Element_t&) override;
        Reduce_Feeder_ComputeType operator()(const std::vector<Reduce_Feeder_ComputeType>&) override;
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
        //std::vector<Fold_Lateral> feeders; // [NUM_FEEDERS];
        //Reduce<LateralFold> feeders; // [NUM_FEEDERS];
        Reduce_Feeder feeders; // [NUM_FEEDERS];
};

#endif // #ifndef POWER_H
