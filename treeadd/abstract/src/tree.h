/* For copyright information, see olden_v1.0/COPYRIGHT */

/* tree.h
 */

#include <stdio.h>
#define chatting printf

#include "Fractal_dynamic.h"

using namespace abstract;

using Fractal_Element_t = class Node;
using Fractal_Seed_t = int;
const size_t Fractal_Arity = 2;
using Fractal_t = Fractal<Fractal_Element_t,
                          Fractal_Seed_t,
                          Fractal_Arity>;

class Node : public Fractal_t::Element {
    
    public:
        
        Node(Fractal_t::ElementInfo info) : Fractal_t::Element(info) {}
        ~Node() {}
        
        void grow() override { val = 1; }

        int val;
};

using TreeAdd_Compute_t = int;
class TreeAdd : public Fractal_t::ComputeFunction<TreeAdd_Compute_t> {
    public:
        TreeAdd_Compute_t operator()(Node&, const std::vector<TreeAdd_Compute_t>&) override;
};

//
