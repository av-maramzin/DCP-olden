/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "Fractal.h"

using namespace abstract;

extern int NumNodes;
#define chatting      printf

#define NULL 0

typedef enum {black, white, grey} Color;
typedef enum {northwest, northeast, southwest, southeast} ChildType;
typedef enum {north, east, south, west} Direction;

typedef struct quad_struct {
    Color color;
    ChildType childtype;
    int size;

  // [*] Remove linking information out of fractal data element
//  struct quad_struct *nw;
//  struct quad_struct *ne;
//  struct quad_struct *sw;
//  struct quad_struct *se;
//  struct quad_struct *parent;
} quad_struct, *QuadTree;

const unsigned int ChildrenNum = 4;

using FractalElementData_t = struct quad_struct;
using FractalElement_t = FractalElement<FractalElementData_t,ChildrenNum>;

struct growth_seed {
    int size;
    int center_x;
    int center_y;
    int lo_proc;
    int hi_proc;
    ChildType ct;
};
using GrowthSeed_t = struct growth_seed;

//QuadTree MakeTree(int size, int center_x, int center_y, int lo_proc,
//                  int hi_proc, QuadTree parent, ChildType ct, int level);

using GrowthFunc_t = void (*)(FractalElementData_t*, const FractalElementInfo&, GrowthSeed_t); 
void growth_func(FractalElementData_t* elem, const FractalElementInfo& info, GrowthSeed_t seed);

using NextGrowthSeedFunc_t = void (*)(const GrowthSeed_t&, GrowthSeed_t&, int);
void next_growth_seed_func(const GrowthSeed_t& parent_seed, GrowthSeed_t& next_child_seed, int next_child_num);

using GrowthStopFunc_t = bool (*)(const FractalElementInfo&, const GrowthSeed_t&);
bool growth_stop_func(const FractalElementInfo& info, const GrowthSeed_t& growth_seed);

// [*] Apply processing functions to the built Fractal
// Functions to be passed into Fractal::apply()
// These functions take Fractal element and process it

//int CountTree(QuadTree tree); 
using FractalApply_CountTree_ret_t = int;
using FractalApply_CountTree_func_t = FractalApply_CountTree_ret_t (*)(FractalElementData_t* tree,
                                                                       const std::vector<FractalApply_CountTree_ret_t>& child_rets);
FractalApply_CountTree_ret_t CountTree(FractalElementData_t* tree, const std::vector<FractalApply_CountTree_ret_t>& child_rets);

// [*] Walk over built Fractal
// Functions to be passed into Fractal::walk()
// These functions take Fractal element and process it
// along with surrounding elements

//int perimeter(QuadTree tree, int size)
using FractalWalk_perimeter_ret_t = int;
using FractalWalk_perimeter_func_t = FractalWalk_perimeter_ret_t (*)(FractalElement_t* tree,
                                                                     const std::vector<FractalWalk_perimeter_ret_t>& child_rets);
FractalWalk_perimeter_ret_t perimeter(FractalElement_t* tree, const std::vector<FractalWalk_perimeter_ret_t>& child_rets);

//
