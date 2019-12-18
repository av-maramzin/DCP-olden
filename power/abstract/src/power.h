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

#ifndef POWER_H
#define POWER_H

#include <vector>
#include <list>

namespace power {

class Demand {

    public:
        Demand(double _P = 0, double _Q = 0) : P(_P), Q(_Q) {}
        
        Demand(const Demand& d) {
            P = d.P;
            Q = d.Q;
        }

        Demand& operator+=(Demand d) {
            P += d.P;
            Q += d.Q;
            return *this;
        }

        double P { 0.0 };
        double Q { 0.0 };
};

class Coefficients {

    public:
        Coefficients(double _alpha, double _beta,
                     double _R, double _X) 
            : alpha(_alpha), beta(_beta), R(_R), X(_X), pi_R(0), pi_I(0) {}

        void recompute_alpha_beta(const double P, const double Q) {
            double a = 2*R*P;
            double b = 2*X*Q;
            
            alpha = a/(1-a-b);
            beta = b/(1-a-b);
        }

        double recompute_root(const double P, const double Q) {
            double a, b, c;
            a = R*R + X*X;  
            b = 2*R*X*Q - 2*X*X*P - R;
            c = R*Q - X*P;
            c = c*c + R*P;
            return (-b-sqrt(b*b-4*a*c))/(2*a);
        }

        void recompute_pi(const double pi_R, const double pi_I,
                          const double theta_R, const double theta_I,  
                          double& new_pi_R, double& new_pi_I) {
            new_pi_R = pi_R + alpha*(theta_R+(theta_I*X)/R);
            new_pi_I = pi_I + beta*(theta_I+(theta_R*R)/X);
            this->pi_R = new_pi_R;
            this->pi_I = new_pi_I;
        }

        double alpha;
        double beta;
        double R;
        double X;
        double pi_R;
        double pi_I;
};

class Leaf : public Demand {

    public:
        Demand compute(double pi_R, double pi_I);

        double pi_R { 1.0 };
        double pi_I { 1.0 };
};

class Branch : public Demand, public Coefficients {

    public:
        Branch(const int num_leaves_per_branch);
        
        Demand compute(Demand branches_cumulative,
                       double theta_R, double theta_I);
        
        const int num_leaves_per_branch;
        std::vector<Leaf> leaves;
};

class Lateral : public Demand, public Coefficients {

    public:
        Lateral(const int num_branches_per_lateral, 
                const int num_leaves_per_branch);

        Demand compute(Demand laterals_cumulative, 
                       double theta_R, double theta_I);

        const int num_branches_per_lateral;
        std::list<Branch> branches;
};

class Root {

    public:
        Root(const int num_feeders, const int num_laterals_per_feeder, 
             const int num_branches_per_lateral, const int num_leaves_per_branch);

        void compute();

        Demand D;
        double theta_R; 
        double theta_I; 
        
        Demand last;
        double last_theta_R; 
        double last_theta_I;
        
        const int num_laterals_per_feeder;
        const int num_feeders;
        std::vector<std::list<Lateral>> feeders;
};

} // namespace Power

int power_pricing_problem();

#endif // #ifndef POWER_H
