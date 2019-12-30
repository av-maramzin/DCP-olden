/* For copyright information, see olden_v1.0/COPYRIGHT */

/* tree-alloc.c
 */

#include "tree.h"
//extern void *malloc(unsigned);

void growth_func(FractalElement_t* tree_node, const FractalElementInfo& info) {
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

