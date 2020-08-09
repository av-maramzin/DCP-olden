/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "Fractal_dynamic.h"

using namespace abstract;

extern int NumNodes;
#define chatting      printf

#define NULL 0

typedef enum { black, white, grey } Color;
typedef enum { northwest, northeast, southwest, southeast } ChildType;
typedef enum { north, east, south, west } Direction;

// Fractal

using Fractal_Element_t = class QuadStruct;
struct growth_seed {
    int size;
    int center_x;
    int center_y;
    int lo_proc;
    int hi_proc;
    ChildType ct;
};
using Fractal_Seed_t = struct growth_seed;
const size_t Fractal_Arity = 4;
using Fractal_t = Fractal<Fractal_Element_t,Fractal_Seed_t,Fractal_Arity>;

class QuadStruct : public Fractal_t::Element {

    public:
        
        QuadStruct(const Fractal_t::ElementInfo& info) : Fractal_t::Element(info) {}
        ~QuadStruct() {}

        void grow(Fractal_t::Seed_t seed) override;
        bool growth_stop_condition() override;
        Fractal_t::Seed_t spawn_child_seed(int child_id) override;

        Color color;
        ChildType childtype;
        int size;
};

using CountTree_Compute_t = int;
class CountTree : public Fractal_t::ComputeFunction<CountTree_Compute_t> {
    public:
        Compute_t operator()(QuadStruct& tree, const std::vector<Compute_t>& child_rets);
};

using Perimeter_Compute_t = int;
class Perimeter : public Fractal_t::ComputeFunction<Perimeter_Compute_t> {
    public:
        Compute_t operator()(QuadStruct& tree, const std::vector<Compute_t>& child_rets);
};

//
