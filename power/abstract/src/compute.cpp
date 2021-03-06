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
#pragma omp threadprivate(P,Q)

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

// Root compute

Demand Root::compute() {
    // final compute value
    Demand a;
    // prepare an injection
    Reduce_Feeder::Inject_t inject_data;
    inject_data.theta_R = this->theta_R; 
    inject_data.theta_I = this->theta_I; 
    inject_data.pi_R = this->theta_R; 
    inject_data.pi_I = this->theta_I; 
    
    //printf("INJECT Root: theta_R=%4.2f, theta_I=%4.2f\n, pi_R=%4.2f, pi_I=%4.2f\n", 
    //        this->theta_R, this->theta_I, this->theta_R, this->theta_I);
    
    // do an injection into the reduction
    if (parallel) {
        feeders.set_impl_type(Reduce_Feeder::ImplType::parallel);
    } else {
        feeders.set_impl_type(Reduce_Feeder::ImplType::sequential);
    }
    feeders.inject(inject_data);
    // create a compute function object and call the API 
    Feeder_ComputeFunc compute_func;
    if (parallel) {
        feeders.set_impl_type(Reduce_Feeder::ImplType::parallel);
    } else {
        feeders.set_impl_type(Reduce_Feeder::ImplType::sequential);
    }
    a = feeders.template compute<Demand>(compute_func);
    // update the Root
    this->D.P = a.P;
    this->D.Q = a.Q;
    //printf("COMPUTE Root: D.P=%4.2f, D.Q=%4.2f\n", 
    //        a.P, a.Q);
    // return computed result
    return a;
}

// Feeder compute

void Feeder::inject(const Reduce_Feeder::Inject_t data) {
    
    Fold_Lateral::Inject_t inject_data;
    inject_data.theta_R = data.theta_R; 
    inject_data.theta_I = data.theta_I; 
    inject_data.pi_R = data.pi_R; 
    inject_data.pi_I = data.pi_I; 

    //printf("INJECT Feeder: theta_R=%4.2f, theta_I=%4.2f\n, pi_R=%4.2f, pi_I=%4.2f\n", 
    //        data.theta_R, data.theta_I, data.pi_R, data.pi_I);

    fold_lateral.inject(inject_data);
}

Feeder_ComputeFunc::Compute_t 
Feeder_ComputeFunc::operator()(Reduce_Feeder::Element_t& feeder) {
    Lateral_ComputeFunc compute_func;
    Demand D = feeder.fold_lateral.template compute<Feeder_ComputeFunc::Compute_t>(compute_func);
    //printf("COMPUTE Lateral Fold [%d] = {P=%4.2f, Q=%4.2f}\n", feeder.element_info().index, D.P, D.Q); 
    return D;
}

Feeder_ComputeFunc::Compute_t 
Feeder_ComputeFunc::operator()(std::vector<Feeder_ComputeFunc::Compute_t>& rets) {
    Demand tmp;
    tmp.P = 0.0;
    tmp.Q = 0.0;
    for (auto ret : rets) {
        tmp.P += ret.P;
        tmp.Q += ret.Q;
    }
    return tmp;
}

// Lateral compute

Lateral_ComputeFunc::Compute_t Lateral_ComputeFunc::operator()(Fold_Lateral::Element_t& element, 
                                          Lateral_ComputeFunc::Compute_t cumulative)
{
    // copy data from the element
    Demand& D = element.D;
    double alpha = element.alpha;
    double beta = element.beta;
    double R = element.R;
    double X = element.X;
    Fold_Branch& branch_fold = element.fold_branch;

    Demand a1;
    Demand a2;
 
    Fold_Lateral::Inject_t inject_data = element.get_injected_data();
    Fold_Branch::Inject_t injection;
    injection.theta_R = inject_data.theta_R;
    injection.theta_I = inject_data.theta_I;
    injection.pi_I = inject_data.pi_I;
    injection.pi_R = inject_data.pi_R;
    element.fold_branch.inject(injection);
 
    a1 = (struct demand) cumulative;
   
    Branch_ComputeFunc compute_func;
    a2 = element.fold_branch.template compute<Demand>(compute_func);

    D.P = a1.P + a2.P;
    D.Q = a1.Q + a2.Q;

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
    
    element.alpha = a/(1-a-b);
    element.beta = b/(1-a-b);
  
    //printf("COMPUTE Lateral [%d] = {P=%4.2f, Q=%4.2f}\n", element.element_info().index, D.P, D.Q); 
    
    return D;
}

Fold_Lateral::Inject_t Lateral::inject(const Fold_Lateral::Inject_t injected_data)
{
    double theta_R;
    double theta_I;
    double pi_R;
    double pi_I;

    theta_R = injected_data.theta_R;
    theta_I = injected_data.theta_I;
    pi_R = injected_data.pi_R;
    pi_I = injected_data.pi_I;

    // recompute new pi values
    double new_pi_R, new_pi_I;
    new_pi_R = pi_R + alpha*(theta_R+(theta_I*X)/R);
    new_pi_I = pi_I + beta*(theta_I+(theta_R*R)/X);

    // plant current seed to be later used in computations
    Fold_Lateral::Inject_t current_elem_seed;
    current_elem_seed.theta_R = injected_data.theta_R;
    current_elem_seed.theta_I = injected_data.theta_I;
    current_elem_seed.pi_R = new_pi_R;
    current_elem_seed.pi_I = new_pi_I;
    this->plant_injected_data(current_elem_seed);
 
    //printf("INJECT Lateral: theta_R=%4.2f, theta_I=%4.2f\n, pi_R=%4.2f, pi_I=%4.2f\n", 
    //        current_elem_seed.theta_R, current_elem_seed.theta_I, current_elem_seed.pi_R, current_elem_seed.pi_I);
   
    // pass the new seed to the next fold element
    return current_elem_seed;
}

// Branch compute

Branch_ComputeFunc::Compute_t Branch_ComputeFunc::operator()(Fold_Branch::Element_t& element, 
                                         Branch_ComputeFunc::Compute_t cumulative)
{
    // copy data from the element
    Demand& D = element.D;
    double alpha = element.alpha;
    double beta = element.beta;
    double R = element.R;
    double X = element.X;

    // reduce all leaves
    Fold_Branch::Inject_t fold_inject_data = element.get_injected_data();
    Reduce_Leaf::Inject_t leaf_inject_data;
    leaf_inject_data.pi_R = fold_inject_data.pi_R;
    leaf_inject_data.pi_I = fold_inject_data.pi_I;
    
    if (parallel) {
        element.leaves.set_impl_type(Reduce_Leaf::ImplType::parallel);
    } else {
        element.leaves.set_impl_type(Reduce_Leaf::ImplType::sequential);
    }
    element.leaves.inject(leaf_inject_data);
   
    Leaf_ComputeFunc compute_func; 
    Demand tmp;
    if (parallel) {
        element.leaves.set_impl_type(Reduce_Leaf::ImplType::parallel);
    } else {
        element.leaves.set_impl_type(Reduce_Leaf::ImplType::sequential);
    }
    tmp = element.leaves.template compute<Leaf_ComputeFunc::Compute_t>(compute_func);
    
    //printf("COMPUTE Leaves [%d] = {P=%4.2f, Q=%4.2f}\n", element.element_info().index, tmp.P, tmp.Q); 
    
    D.P = cumulative.P + tmp.P;
    D.Q = cumulative.Q + tmp.Q;

    /* compute P,Q */
    double a, b, c, root;
    
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
    
    element.alpha = a/(1-a-b);
    element.beta = b/(1-a-b);

    //printf("COMPUTE Branch [%d] = {P=%4.2f, Q=%4.2f}\n", element.element_info().index, D.P, D.Q); 

    return D;
}

Fold_Branch::Inject_t Branch::inject(Fold_Branch::Inject_t injected_data)
{
    double theta_R;
    double theta_I;
    double pi_R;
    double pi_I;

    theta_R = injected_data.theta_R;
    theta_I = injected_data.theta_I;
    pi_R = injected_data.pi_R;
    pi_I = injected_data.pi_I;

    // recompute new pi values
    double new_pi_R, new_pi_I;
    new_pi_R = pi_R + this->alpha*(theta_R+(theta_I*this->X)/this->R);
    new_pi_I = pi_I + this->beta*(theta_I+(theta_R*this->R)/this->X);

    // plant current seed to be later used in computations
    Fold_Branch::Inject_t current_elem_seed;
    current_elem_seed.theta_R = injected_data.theta_R;
    current_elem_seed.theta_I = injected_data.theta_I;
    current_elem_seed.pi_R = new_pi_R;
    current_elem_seed.pi_I = new_pi_I;
    this->plant_injected_data(current_elem_seed);
 
    //printf("INJECT Branch: theta_R=%4.2f, theta_I=%4.2f\n, pi_R=%4.2f, pi_I=%4.2f\n", 
    //        current_elem_seed.theta_R, current_elem_seed.theta_I, current_elem_seed.pi_R, current_elem_seed.pi_I);
  
    // pass the new seed to the next fold element
    return current_elem_seed;
}

Leaf_ComputeFunc::Compute_t Leaf_ComputeFunc::operator()(Reduce_Leaf::Element_t& element)
{
    P = element.D.P;
    Q = element.D.Q;
 
    Reduce_Leaf::Inject_t injected_data = element.get_injected_data();

    //printf("INJECT Leaf: pi_R=%4.2f, pi_I=%4.2f\n", 
    //        injected_data.pi_R, injected_data.pi_I);

    optimize_node(injected_data.pi_R, injected_data.pi_I);

    if (P < 0.0) {
        P = 0.0;
        Q = 0.0;
    }
    
    element.D.P = P;
    element.D.Q = Q;

    //printf("COMPUTE Leaf [%d] = {P=%4.2f, Q=%4.2f}\n", element.element_info().index, element.D.P, element.D.Q); 

    return element.D;
}

Leaf_ComputeFunc::Compute_t Leaf_ComputeFunc::operator()(std::vector<Leaf_ComputeFunc::Compute_t>& rets)
{
    Demand tmp;
    tmp.P = 0.0;
    tmp.Q = 0.0;
    
    for (auto ret : rets) {
        tmp.P += ret.P;
        tmp.Q += ret.Q;
    }

    return tmp;
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

double make_orthogonal(double* v_mod, double* v_static)
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
