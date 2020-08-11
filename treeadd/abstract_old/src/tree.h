/* For copyright information, see olden_v1.0/COPYRIGHT */

/* tree.h
 */

#include <stdio.h>
#include <stdlib.h>
#define chatting printf
#define PLAIN

#include <vector>

#include "Fractal_static.h"

using namespace abstract;
using namespace std;

//

typedef struct tree {
    int val;
    //struct tree *left, *right;
} tree_t;
using FractalElementData_t = struct tree;
using FractalElement_t = FractalElement<FractalElementData_t,2>;
using GrowthSeed_t = int;

// [*] Build the Fractal of tree nodes
// Call Fractal::grow() instead
//extern tree_t *TreeAlloc (int level, int lo, int hi);

using GrowthFunc_t = void (*)(FractalElementData_t*, const FractalElementInfo&, GrowthSeed_t); 
void growth_func(FractalElementData_t* elem, const FractalElementInfo& info, GrowthSeed_t seed);

using NextGrowthSeedFunc_t = void (*)(const GrowthSeed_t&, GrowthSeed_t&, int);
void next_growth_seed_func(const GrowthSeed_t& parent_seed, GrowthSeed_t& next_child_seed, int next_child_num);

using GrowthStopFunc_t = bool (*)(const FractalElementInfo&, const GrowthSeed_t&);
bool growth_stop_func(const FractalElementInfo& info, const GrowthSeed_t& growth_seed);

// [*] Apply processing functions to the built Fractal
// Functions to be passed into Fractal::apply()
// These functions take Fractal element and process it

//int TreeAdd (int inc_level, tree_t *t);
using FractalApply_TreeAdd_ret_t = int;
using FractalApply_TreeAdd_func_t = FractalApply_TreeAdd_ret_t (*)(FractalElementData_t* tree_node,
                                                              const std::vector<FractalApply_TreeAdd_ret_t>& child_rets);
FractalApply_TreeAdd_ret_t TreeAdd(FractalElementData_t* tree_node, const std::vector<FractalApply_TreeAdd_ret_t>& child_rets);

