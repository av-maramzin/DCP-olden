/* For copyright information, see olden_v1.0/COPYRIGHT */

/* compute.c
 *
 * By:  Martin C. Carlisle
 * 6/15/94
 *
 * Implements computation phase of the Power Pricing problem
 * based on code by: Steve Lumetta, Sherry Li, and Ismail Khalil
 * University of California at Berkeley
 *
 */

#include "power.h"

/*----------------------------------------------------------------------*/
/* Leaf optimization 'global' variables               */

static double P=1.0;
static double Q=1.0;


/*----------------------------------------------------------------------*/
/* Leaf optimization procedures                 */

void optimize_node(double pi_R, double pi_I);
double find_g();
double find_h();
double find_gradient_f(double pi_R, double pi_I, double* gradient);
double find_gradient_g(double* gradient);
double find_gradient_h(double* gradient);
void find_dd_grad_f(double pi_R, double pi_I, double* dd_grad);
double make_orthogonal(double* v_mod, double* v_static);

Demand Root::compute() {
    Demand a;
    Demand tmp;

    tmp.P = 0.0;
    tmp.Q = 0.0;
    for (int i = 0; i < NUM_FEEDERS; i++) {
        LateralFold& l = feeders[i];
        
        // prepare a seed and do injection
        LateralFold::Seed_t seed;
        seed.theta_R = this->theta_R; 
        seed.theta_I = this->theta_I; 
        seed.pi_R = this->theta_R; 
        seed.pi_I = this->theta_I; 

        l.inject(seed);
        
        a = l.compute();
        tmp.P += a.P;
        tmp.Q += a.Q;
    }
    this->D.P = tmp.P;
    this->D.Q = tmp.Q;
}

LateralFold::Compute_t Lateral::compute(LateralFold::Compute_t fold) {
    
    Demand a1;
    Demand a2;
  
    a1 = fold;

    branch_fold.inject(this->extract_seed());
    a2 = branch_fold.compute();

    this->D.P = a1.P + a2.P;
    this->D.Q = a1.Q + a2.Q;

    double a, b, c, root;
    
    /* compute P,Q */
    a = R*R + X*X;  
    b = 2*R*X*D.Q - 2*X*X*D.P - R;
    c = R*D.Q - X*D.P;
    c = c*c + R*D.P;
    root = (-b-sqrt(b*b-4*a*c))/(2*a);
    
    D.Q = D.Q + ((root-D.P)*X)/R;
    D.P = root;

    /* compute alpha, beta */
    a = 2*R*D.P;
    b = 2*X*D.Q;
    
    alpha = a/(1-a-b);
    beta = b/(1-a-b);
    
    return D;
}

LateralFold::Seed_t Lateral::inject(LateralFold::Seed_t seed)
{
    double theta_R;
    double theta_I;
    double pi_R;
    double pi_I;

    theta_R = seed.theta_R;
    theta_I = seed.theta_I;
    pi_R = seed.pi_R;
    pi_I = seed.pi_I;

    // recompute new pi values
    double new_pi_R, new_pi_I;
    new_pi_R = pi_R + l->alpha*(theta_R+(theta_I*l->X)/l->R);
    new_pi_I = pi_I + l->beta*(theta_I+(theta_R*l->R)/l->X);

    // plant current seed to be later used in computations
    LateralFold::Seed_t current_elem_seed;
    current_elem_seed.theta_R = seed.theta_R;
    current_elem_seed.theta_I = seed.theta_I;
    current_elem_seed.pi_R = new_pi_R;
    current_elem_seed.pi_I = new_pi_I;
    this->plant_seed(current_elem_seed);
   
    // pass the new seed to the next fold element
    return current_elem_seed;
}

BranchFold::Compute_t Branch::compute(BranchFold::Compute_t fold) {
    Demand a2,tmp;
    double new_pi_R, new_pi_I;
    double a,b,c,root;
    Leaf l;
    Branch next;
    int i;
    Demand a1;
  
    /* Initialize tmp */
    tmp.P = 0.0;
    tmp.Q = 0.0;
    for (int i=0; i<LEAVES_PER_BRANCH; i++) {
        a2 = leaves[i]->compute(seed.pi_R, seed.pi_I);
        tmp.P += a2.P;
        tmp.Q += a2.Q;
    }
    
    D.P = fold.P + tmp.P;
    D.Q = fold.Q + tmp.Q;

    /* compute P,Q */
    a = R*R + X*X;  
    b = 2*R*X*D.Q - 2*X*X*D.P - R;
    c = R*D.Q - X*D.P;
    c = c*c + R*D.P;
    root = (-b-sqrt(b*b-4*a*c))/(2*a);
    
    D.Q = D.Q + ((root-D.P)*X)/R;
    D.P = root;
    
    /* compute alpha, beta */
    a = 2*R*D.P;
    b = 2*X*D.Q;
    alpha = a/(1-a-b);
    beta = b/(1-a-b);

    return D;
}

BranchFold::Seed_t Branch::inject(BranchFold::Seed_t seed)
{
    double theta_R;
    double theta_I;
    double pi_R;
    double pi_I;

    theta_R = seed.theta_R;
    theta_I = seed.theta_I;
    pi_R = seed.pi_R;
    pi_I = seed.pi_I;

    // recompute new pi values
    double new_pi_R, new_pi_I;
    new_pi_R = pi_R + this->alpha*(theta_R+(theta_I*this->X)/this->R);
    new_pi_I = pi_I + this->beta*(theta_I+(theta_R*this->R)/this->X);

    // plant current seed to be later used in computations
    BranchFold::Seed_t current_elem_seed;
    current_elem_seed.theta_R = seed.theta_R;
    current_elem_seed.theta_I = seed.theta_I;
    current_elem_seed.pi_R = new_pi_R;
    current_elem_seed.pi_I = new_pi_I;
    this->plant_seed(current_elem_seed);
   
    // pass the new seed to the next fold element
    return current_elem_seed;
}

Demand Leaf::compute(double pi_R, double pi_I) {
    
    P = this->D.P;
    Q = this->D.Q;
  
    optimize_node(pi_R, pi_I);

    if (P<0.0) {
        P = 0.0;
        Q = 0.0;
    }
    
    this->D.P = P;
    this->D.Q = Q;
  
    return this->D;
}

/*----------------------------------------------------------------------*/

void optimize_node (double pi_R, double pi_I)
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
	    P-=total*grad_h[0];
	    Q-=total*grad_h[1];
	}

	/* Check that g is still valid */
	g=find_g ();
	if (g>G_EPSILON) {
	    magnitude=find_gradient_g (grad_g);
	    find_gradient_h (grad_h);
	    magnitude*=make_orthogonal (grad_g,grad_h);
	    total=g/magnitude;
	    P-=total*grad_g[0];
	    Q-=total*grad_g[1];
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
	P+=magnitude*grad_f[0];
	Q+=magnitude*grad_f[1];

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
    return (P*P+Q*Q-0.8);
}

double find_h ()
{
    return (P-5*Q);
}

double find_gradient_f (double pi_R, double pi_I, double* gradient)
{
    int		    i;
    double	    magnitude=0.0;

    gradient[0]=1/(1+P)-pi_R;
    gradient[1]=1/(1+Q)-pi_I;
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

    gradient[0]=2*P;
    gradient[1]=2*Q;
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
    double	    P_plus_1_inv=1/(P+1);
    double	    Q_plus_1_inv=1/(Q+1);
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

/*----------------------------------------------------------------------*/
