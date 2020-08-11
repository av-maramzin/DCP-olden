/* For copyright information, see olden_v1.0/COPYRIGHT */

/* tree-alloc.c
 */

#include "tree.h"
//extern void *malloc(unsigned);

void growth_func(FractalElementData_t* tree_node, const FractalElementInfo& info, GrowthSeed_t s) {
//tree_t *TreeAlloc (int level, int lo, int proc) {
//  if (level == 0)
//    return NULL;
//  else {
//    struct tree *new, *right, *left;
//    new = (struct tree *) malloc(sizeof(tree_t));
//    left = TreeAlloc(level-1, lo+proc/2, proc/2);
//    right=TreeAlloc(level-1,lo,proc/2);
    tree_node->val = 1;
//    new->left = (struct tree *) left;
//    new->right = (struct tree *) right;
//    return new;
//  }
    return;
}

void next_growth_seed_func(const GrowthSeed_t& parent_seed, GrowthSeed_t& next_child_seed, int next_child_num)
{
    return;
}

bool growth_stop_func(const FractalElementInfo& info, const GrowthSeed_t& growth_seed)
{
    return false;
}

//
