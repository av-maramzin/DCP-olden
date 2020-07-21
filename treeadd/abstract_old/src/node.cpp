/* For copyright information, see olden_v1.0/COPYRIGHT */

/* node.c
 */

#include <stdlib.h>
#include "tree.h"

extern int NumNodes;

extern int runs;

int dealwithargs(int argc, char *argv[]);

//typedef struct {
//    long 	level;
//} startmsg_t;

extern void growth_func(FractalElement_t*,const FractalElementInfo&);

int main (int argc, char *argv[])
{
//    tree_t	*root;
    int level,result;

    level = dealwithargs(argc, argv);
    chatting("Treeadd with %d levels on %d processors with %d runs\n",
	     level, NumNodes, runs);
    /* only processor 0 will continue here. */
    chatting("About to enter TreeAlloc\n");

    using FractalGrowthFunc_t = void (*)(FractalElement_t*,const FractalElementInfo&);
    int scaling_factor = 1;
    Fractal<FractalElement_t,FractalGrowthFunc_t> fractal(growth_func, scaling_factor);
    
//    root = TreeAlloc (level, 0, NumNodes);
    int fractal_levels = level;
    int fractal_seed_label = 0;
    fractal.grow(fractal_levels,fractal_seed_label);    

    chatting("About to enter TreeAdd\n");

#ifndef PLAIN
    ClearAllStats();
#endif
    {
        int i;
        for (i = 0; i < runs; ++i) {
//            result = TreeAdd (0, root);
            result = fractal.template apply<FractalApply_TreeAdd_func_t,FractalApply_TreeAdd_ret_t>(TreeAdd);
        }
    }

    chatting("Received result of %d\n",result);

    exit(0);
}

/* TreeAdd:
 */
FractalApply_TreeAdd_ret_t TreeAdd(FractalElement_t* t, const std::vector<FractalApply_TreeAdd_ret_t>& child_rets)
{
//  if (t == NULL)  {
//    return 0;
//  }
//  else {
//    int leftval;
//    int rightval;
//    tree_t *tleft, *tright;
//    int value;

//    tleft = t->left;            /* <---- 57% load penalty */
//    leftval = TreeAdd(inc_level + 1, tleft);
//    tright = t->right;          /* <---- 11.4% load penalty */
//    rightval = TreeAdd(inc_level + 1, tright);
//    /*chatting("after touch\n");*/
//    value = t->val;
    /*chatting("returning from treeadd %d\n",*/
	     /*leftval.value + rightval.value + value);*/

    FractalApply_TreeAdd_ret_t ret_value = 0;

    for (auto child_val : child_rets) {
        ret_value += child_val;
    }

    ret_value += t->val;
    
    return ret_value;
//    return leftval + rightval + value;
//  }
} /* end of TreeAdd */

