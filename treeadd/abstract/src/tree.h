/* For copyright information, see olden_v1.0/COPYRIGHT */

/* tree.h
 */

#include <stdio.h>
#include <stdlib.h>
#define chatting printf
#define PLAIN

#include <vector>

#include "Fractal.h"

using namespace abstract;
using namespace std;

typedef struct tree {
    int val;
    //struct tree *left, *right;
} tree_t;
using FractalElement_t = struct tree;

// [*] Build the Fractal of tree nodes
// Call Fractal::grow() instead
//extern tree_t *TreeAlloc (int level, int lo, int hi);

// [*] Apply processing functions to the built Fractal
// Functions to be passed into Fractal::apply()
// These functions take Fractal element and process it

//int TreeAdd (int inc_level, tree_t *t);
using FractalApply_TreeAdd_ret_t = int;
using FractalApply_TreeAdd_func_t = FractalApply_TreeAdd_ret_t (*)(FractalElement_t* tree_node,
                                                              const std::vector<FractalApply_TreeAdd_ret_t>& child_rets);
FractalApply_TreeAdd_ret_t TreeAdd(FractalElement_t* tree_node, const std::vector<FractalApply_TreeAdd_ret_t>& child_rets);

