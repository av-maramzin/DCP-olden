/* For copyright information, see olden_v1.0/COPYRIGHT */

/* node.c
 */

#include <stdlib.h>
#include "tree.h"

extern int NumNodes;

extern int runs;

extern bool parallel;
extern bool balanced;

int dealwithargs(int argc, char *argv[]);

int main(int argc, char* argv[])
{
    int level;
    int result;

    level = dealwithargs(argc, argv);
    chatting("Treeadd with %d levels on %d processors with %d runs\n",
	     level, NumNodes, runs);
    /* only processor 0 will continue here. */
    chatting("About to enter TreeAlloc\n");

    Fractal_t tree;
    
    if (parallel) {
        tree.set_impl_type(Fractal_t::ImplType::parallel);
        chatting("parallel\n");
    } else {
        tree.set_impl_type(Fractal_t::ImplType::sequential);
        chatting("sequential\n");
    }

    if (balanced) {
        tree.set_type(Fractal_t::Type::balanced);
        chatting("balanced\n");
    } else {
        tree.set_type(Fractal_t::Type::unbalanced);
        chatting("unbalanced\n");
    }
    
    tree.grow(level-1);

    chatting("About to enter TreeAdd\n");

    {
        TreeAdd comp_func;
        int i;
        for (i = 0; i < runs; ++i) {
            result = tree.template compute<int>(comp_func);
        }
    }

    chatting("Received result of %d\n", result);

    return 0;
}

int TreeAdd::operator()(Node& elem, const std::vector<int>& child_rets)
{
    int value = elem.val;
    for (auto val : child_rets) {
        value += val;
    }
    return value;
}

//
