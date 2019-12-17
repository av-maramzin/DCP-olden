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

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <omp.h>
#include <fenv.h>

#include <list>
#include <vector>
#include <iostream>
using namespace std;

#include "power.h"
#include "defines.h"

namespace power {

static double _P = 1.0;
static double _Q = 1.0;
#pragma omp threadprivate(_P, _Q)

// former build.cpp
Root::Root(const int num_feeders, 
           const int num_laterals_per_feeder, 
           const int num_branches_per_lateral,
           const int num_leaves_per_branch)
    : num_laterals_per_feeder{num_laterals_per_feeder}, 
      num_feeders{num_feeders},
      theta_R{0.8}, theta_I{0.16}
{
    feeders.reserve(num_feeders);
    for (auto i = 0; i < num_feeders; i++) {
        list<Lateral> lateralsList;
        
        for (auto j = 0; j < num_laterals_per_feeder; j++) {
            lateralsList.push_back(
                Lateral(num_branches_per_lateral, num_leaves_per_branch)
            );
        }
        
        feeders.push_back(lateralsList);
    }
}

Lateral::Lateral(const int num_branches_per_lateral, 
                 const int num_leaves_per_branch)
    : Coefficients(0.0, 0.0, 1/300000.0, 0.000001), 
      num_branches_per_lateral{num_branches_per_lateral}
{
    for (auto i = 0; i < num_branches_per_lateral; i++) {
        branches.push_back(
            Branch(num_leaves_per_branch)
        );
    }
}

Branch::Branch(const int num_leaves_per_branch)
    : Coefficients(0.0, 0.0, 0.0001, 0.00002), 
      num_leaves_per_branch{num_leaves_per_branch}
{
    for (auto i = 0; i < num_leaves_per_branch; i++) {
        leaves.push_back(
            Leaf()
        );
    }
}

// former compute.cpp
void Root::compute() {
    D.P = 0.0;
    D.Q = 0.0;
    
    double sum_P = 0.0;
    double sum_Q = 0.0;
#pragma omp parallel for reduction(+:sum_P,sum_Q)
    for (int i = 0; i < feeders.size(); i++) {
        std::list<Lateral>& laterals_list = feeders[i];
        
        double new_pi_R, pi_R = theta_R;
        double new_pi_I, pi_I = theta_I;
        for (std::list<Lateral>::iterator lateral = laterals_list.begin(); lateral != laterals_list.end(); lateral++) {
            lateral->recompute_pi(pi_R, pi_I, theta_R, theta_I, new_pi_R, new_pi_I);
            pi_R = new_pi_R;
            pi_I = new_pi_I;
        }

        Demand laterals_cumulative;
        for (auto lateral = laterals_list.rbegin(); lateral != laterals_list.rend(); lateral++) {
            laterals_cumulative = lateral->compute(laterals_cumulative, theta_R, theta_I);
        }
        
        sum_P += laterals_cumulative.P;
        sum_Q += laterals_cumulative.Q;
    }
    D.P = sum_P;
    D.Q = sum_Q;
}

Demand Lateral::compute(Demand laterals_cumulative, 
                        double theta_R, double theta_I) 
{
    double new_pi_R, _pi_R = pi_R;
    double new_pi_I, _pi_I = pi_I;
    for (auto branch = branches.begin(); branch != branches.end(); branch++) {
        branch->recompute_pi(_pi_R, _pi_I, theta_R, theta_I, new_pi_R, new_pi_I);
        _pi_R = new_pi_R;
        _pi_I = new_pi_I;
    }

    Demand branches_cumulative;
    for (auto branch = branches.rbegin(); branch != branches.rend(); branch++) {
        branches_cumulative = branch->compute(branches_cumulative, theta_R, theta_I);
    }
  
    P = laterals_cumulative.P + branches_cumulative.P;
    Q = laterals_cumulative.Q + branches_cumulative.Q;
    
    // compute P,Q
    double root;
    root = recompute_root(P, Q);
    Q = Q + ((root-P)*X)/R;
    P = root;

    // compute alpha, beta
    recompute_alpha_beta(P, Q);

    return Demand(P, Q);
}

Demand Branch::compute(Demand branches_cumulative, 
                       double theta_R, double theta_I) 
{
    Demand leaves_cumulative;
    for (int i = 0; i < num_leaves_per_branch; i++) {
        leaves_cumulative += leaves[i].compute(pi_R, pi_I);
    }

    P = branches_cumulative.P + leaves_cumulative.P;
    Q = branches_cumulative.Q + leaves_cumulative.Q;
    
    // compute P,Q
    double root;
    root = recompute_root(P, Q);
    Q = Q + ((root-P)*X)/R;
    P = root;
  
    // compute alpha, beta
    recompute_alpha_beta(P, Q);

    return Demand(P, Q);
}

// Leaf optimization 'global' variables
void optimize_node (double pi_R, double pi_I);

Demand Leaf::compute(double pi_R, double pi_I) {
    _P = P;
    _Q = Q;
  
    optimize_node(pi_R, pi_I);

    if (_P < 0.0) {
        _P = 0.0;
        _Q = 0.0;
    }
    
    P = _P;
    Q = _Q;
    
    return Demand(P, Q);
}

// Leaf optimization procedures
double find_g ();
double find_h ();
double find_gradient_f (double pi_R, double pi_I, double* gradient);
double find_gradient_g (double* gradient);
double find_gradient_h (double* gradient);
void find_dd_grad_f (double pi_R, double pi_I, double* dd_grad);
double make_orthogonal (double* v_mod, double* v_static);

void optimize_node(double pi_R, double pi_I)
{
    double	    g;
    double	    h;

    double	    grad_f[2];
    double	    grad_g[2];
    double	    grad_h[2];
    double	    dd_grad_f[2];
    double	    magnitude;

    int		    i;
    double	    total;
    double	    max_dist;

    do {
	/* Move onto h=0 line */
	h=find_h ();
	if (fabs (h)>H_EPSILON) {
	    magnitude=find_gradient_h (grad_h);
	    total=h/magnitude;
	    _P-=total*grad_h[0];
	    _Q-=total*grad_h[1];
	}

	/* Check that g is still valid */
	g=find_g ();
	if (g>G_EPSILON) {
	    magnitude=find_gradient_g (grad_g);
	    find_gradient_h (grad_h);
	    magnitude*=make_orthogonal (grad_g,grad_h);
	    total=g/magnitude;
	    _P-=total*grad_g[0];
	    _Q-=total*grad_g[1];
	}

	/* Maximize benefit */
	magnitude=find_gradient_f (pi_R,pi_I,grad_f);
	find_dd_grad_f (pi_R,pi_I,dd_grad_f);
	total=0.0;
	for (i=0; i<2; i++)
	    total+=grad_f[i]*dd_grad_f[i];
	magnitude/=fabs (total);
	find_gradient_h (grad_h);
	magnitude*=make_orthogonal (grad_f,grad_h);
	find_gradient_g (grad_g);
	total=0.0;
	for (i=0; i<2; i++)
	    total+=grad_f[i]*grad_g[i];
	if (total>0) {
	    max_dist=-find_g ()/total;
	    if (magnitude>max_dist)
		magnitude=max_dist;
	}
	_P+=magnitude*grad_f[0];
	_Q+=magnitude*grad_f[1];

	h=find_h ();
	g=find_g ();
	find_gradient_f (pi_R,pi_I,grad_f);
	find_gradient_h (grad_h);

    } while (fabs (h)>H_EPSILON || g>G_EPSILON ||
	    (fabs (g)>G_EPSILON &&
		fabs (grad_f[0]*grad_h[1]-grad_f[1]*grad_h[0])>F_EPSILON));
}

double find_g ()
{
    return (_P*_P+_Q*_Q-0.8);
}

double find_h ()
{
    return (_P-5*_Q);
}

double find_gradient_f (double pi_R, double pi_I, double* gradient)
{
    int		    i;
    double	    magnitude=0.0;

    gradient[0]=1/(1+_P)-pi_R;
    gradient[1]=1/(1+_Q)-pi_I;
    for (i=0; i<2; i++)
	magnitude+=gradient[i]*gradient[i];
    magnitude=sqrt (magnitude);
    for (i=0; i<2; i++)
	gradient[i]/=magnitude;

    return magnitude;
}

double find_gradient_g (double* gradient)
{
    int		    i;
    double	    magnitude=0.0;

    gradient[0]=2*_P;
    gradient[1]=2*_Q;
    for (i=0; i<2; i++)
	magnitude+=gradient[i]*gradient[i];
    magnitude=sqrt (magnitude);
    for (i=0; i<2; i++)
	gradient[i]/=magnitude;

    return magnitude;
}

double find_gradient_h (double* gradient)
{
    int		    i;
    double	    magnitude=0.0;

    gradient[0]=1.0;
    gradient[1]=-5.0;
    for (i=0; i<2; i++)
	magnitude+=gradient[i]*gradient[i];
    magnitude=sqrt (magnitude);
    for (i=0; i<2; i++)
	gradient[i]/=magnitude;

    return magnitude;
}

void find_dd_grad_f (double pi_R, double pi_I, double* dd_grad)
{
    double	    P_plus_1_inv=1/(_P+1);
    double	    Q_plus_1_inv=1/(_Q+1);
    double	    P_grad_term=P_plus_1_inv-pi_R;
    double	    Q_grad_term=Q_plus_1_inv-pi_I;
    double	    grad_mag;
    
    grad_mag=sqrt (P_grad_term*P_grad_term+Q_grad_term*Q_grad_term);

    dd_grad[0]=-P_plus_1_inv*P_plus_1_inv*P_grad_term/grad_mag;
    dd_grad[1]=-Q_plus_1_inv*Q_plus_1_inv*Q_grad_term/grad_mag;
}

double make_orthogonal (double* v_mod, double* v_static)
{
    int		    i;
    double	    total=0.0;
    double	    length=0.0;

    for (i=0; i<2; i++)
	total+=v_mod[i]*v_static[i];
    for (i=0; i<2; i++) {
	v_mod[i]-=total*v_static[i];
	length+=v_mod[i]*v_mod[i];
    }
    length=sqrt (length);
    for (i=0; i<2; i++)
	v_mod[i]/=length;

    if (1-total*total<0)    /* Roundoff error--vectors are parallel */
	return 0;

    return sqrt (1-total*total);
}

} // namespace power

using namespace power;

/* Domain of thetaR->P map is 0.65 to 1.00 [index*0.01+0.65] */
static double map_P[36] =
{8752.218091048, 8446.106670416, 8107.990680283,
 7776.191574285, 7455.920518777, 7146.602181352,
 6847.709026813, 6558.734204024, 6279.213382291,
 6008.702199986, 5746.786181029, 5493.078256495,
 5247.206333097, 5008.828069358, 4777.615815166,
 4553.258735900, 4335.470002316, 4123.971545694,
 3918.501939675, 3718.817618538, 3524.683625800,
 3335.876573044, 3152.188635673, 2973.421417103,
 2799.382330486, 2629.892542617, 2464.782829705,
 2303.889031418, 2147.054385395, 1994.132771399,
 1844.985347313, 1699.475053321, 1557.474019598,
 1418.860479043, 1283.520126656, 1151.338004216};

#define      MIN_THETA_R       0.65
#define      PER_INDEX_R       0.01
#define      MAX_THETA_R       0.995

/* Domain of thetaI->Q map is 0.130 to 0.200 [index*0.002+0.130] */
static double map_Q[36] =
{1768.846590190, 1706.229490046, 1637.253873079,
 1569.637451623, 1504.419525242, 1441.477913810,
 1380.700660446, 1321.980440476, 1265.218982201,
 1210.322424636, 1157.203306183, 1105.780028163,
 1055.974296746, 1007.714103979, 960.930643875,
 915.558722782, 871.538200178, 828.810882006,
 787.322098340, 747.020941334, 707.858376214,
 669.787829741, 632.765987756, 596.751545633,
 561.704466609, 527.587580585, 494.365739051,
 462.004890691, 430.472546686, 399.738429196,
 369.773787595, 340.550287137, 312.041496095,
 284.222260660, 257.068973074, 230.557938283};

#define      MIN_THETA_I       0.13
#define      PER_INDEX_I       0.002
#define      MAX_THETA_I       0.199

static void change_thetas(Root& r) {
    int i;
    double d_theta_R, d_theta_I;

    i = (int)((r.theta_R - MIN_THETA_R) / PER_INDEX_R);
    if (i<0) i=0;
    if (i>35) i=35;
    d_theta_R = -(r.theta_R - r.D.P/10000.0) /
        (1 - (map_P[i+1] - map_P[i]) / (PER_INDEX_R * 10000.0));
    
    i = (int)((r.theta_I - MIN_THETA_I) / PER_INDEX_I);
    if (i<0) i=0;
    if (i>35) i=35;
    d_theta_I = -(r.theta_I - r.D.Q/10000.0) /
        (1 - (map_Q[i+1] - map_Q[i]) / (PER_INDEX_I * 10000.0));
 
    printf("D TR-%4.2f, TI=%4.2f\n", d_theta_R,d_theta_I);
    
    r.last.P = r.D.P;
    r.last.Q = r.D.Q;
    r.last_theta_R = r.theta_R;
    r.last_theta_I = r.theta_I;
    
    r.theta_R = r.theta_R + d_theta_R;
    r.theta_I = r.theta_I + d_theta_I;
}

static bool check_error(Root& r) {
    if ( fabs(r.D.P/10000.0 - r.theta_R) < ROOT_EPSILON &&
         fabs(r.D.Q/10000.0 - r.theta_I) < ROOT_EPSILON ) {
        return true;
    } else {
        return false;
    }
}

int power_pricing_problem()
{
    using namespace power;

    Root root(NUM_FEEDERS, LATERALS_PER_FEEDER, 
              BRANCHES_PER_LATERAL, LEAVES_PER_BRANCH);

    root.compute();
    
    root.last.P = root.D.P;
    root.last.Q = root.D.Q;
    root.last_theta_R = root.theta_R;
    root.last_theta_I = root.theta_I;
    
    root.theta_R = 0.7;
    root.theta_I = 0.14;

    int finished=0;
    while (!finished) {
    
        root.compute(); // recompute the tree 
    
        printf("TR=%4.2f, TI=%4.2f, P0=%4.2f, Q0=%4.2f\n",
            root.theta_R, root.theta_I, root.D.P, root.D.Q);
    
        if (check_error(root)) {
            finished = 1;
        } else {
            change_thetas(root);
        }
    } 
    
    return 0;
}
