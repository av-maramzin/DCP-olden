/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "perimeter.h"
#include <stdio.h>
#include <stdlib.h>

// [*] Utility functions

static int adj(Direction d, ChildType ct)
{
    switch (d) 
    {
    case north:
        return ( (ct==northeast) || (ct==northwest) );
    case south:
        return ( (ct==southeast) || (ct==southwest) );
    case east:
        return ( (ct==northeast) || (ct==southeast) );
    case west:
        return ( (ct==southwest) || (ct==northwest) );
    }
}

static ChildType reflect(Direction d, ChildType ct) 
{
    if ( (d==west) || (d==east) ) 
    {
        switch(ct) 
	{
	    case northwest:
	        return northeast;
	    case northeast:
	        return northwest;
	    case southeast:
	        return southwest;
	    case southwest:
	        return southeast;
#ifdef DEBUG
	    default:
	        printf("\nbug in reflect() \n");
	        exit(1);
#endif
	}
    }
  
    switch(ct) 
    {
        case northwest:
            return southwest;
        case northeast:
            return southeast;
        case southeast:
            return northeast;
        case southwest:
            return northwest;
#ifdef DEBUG
        default:
            printf("\n bug2 in reflect() \n");
            exit(1);
#endif
    }
}

CountTree::Compute_t CountTree::operator()(QuadStruct& tree, const std::vector<CountTree::Compute_t>& child_rets) {
    int ret = 0;

    if (!child_rets.empty()) {
        for (auto child_ret : child_rets) {
            ret += child_ret;
        }
    } else {
        ret = 1;
    }

    return ret;
}

static QuadStruct* child(QuadStruct* tree, ChildType ct)
{
    return static_cast<QuadStruct*>(tree->get_child_ptr((int)ct));
}

static QuadStruct* gtequal_adj_neighbor(QuadStruct* tree, Direction d)
{
    QuadStruct* q;
    QuadStruct* parent;
    ChildType ct;
  
    parent = static_cast<QuadStruct*>(tree->get_parent_ptr());
    ct = tree->childtype;

    if ( (parent != NULL) && adj(d,ct) )
        q = gtequal_adj_neighbor(parent,d);
    else q = parent;

    if (q && q->color==grey) {
        return child(q,reflect(d,ct));
    }
    else return q;
}

static int sum_adjacent(QuadStruct* p, ChildType q1, ChildType q2, int size)
{
    if (p->color == grey) 
    {
        return sum_adjacent(child(p,q1),q1,q2,size/2) +
	    sum_adjacent(child(p,q2),q1,q2,size/2);
    } 
    else if (p->color == white) 
    {
        return size;
    } 
    else return 0;
}

//int perimeter(QuadTree tree, int size)
Perimeter::Compute_t Perimeter::operator()(QuadStruct& tree, const std::vector<Perimeter::Compute_t>& child_rets) {

    int retval = 0;
    QuadStruct* neighbor;

    if (tree.color == grey) {

        if (!tree.has_children()) {
            std::cerr << "Perimeter::operator(): grey node must have children!" << std::endl; 
            std::exit(EXIT_FAILURE);
        }

        for (auto child_ret : child_rets) {
            retval += child_ret;
        }
    
    } else if (tree.color == black) {

        if (tree.has_children()) {
            std::cerr << "Perimeter::operator(): black node cannot have any children!" << std::endl; 
            std::exit(EXIT_FAILURE);
        }

        /* North */
        neighbor = gtequal_adj_neighbor(&tree, north);
        if ((neighbor==NULL) || (neighbor->color == white)) retval += tree.size;
        else if (neighbor->color == grey) { 
	    retval += sum_adjacent(neighbor, southeast, southwest, tree.size);
        }
        /* East */
        neighbor = gtequal_adj_neighbor(&tree,east);
        if ((neighbor==NULL) || (neighbor->color==white)) retval += tree.size;
        else if (neighbor->color==grey) 
	    retval += sum_adjacent(neighbor, southwest, northwest, tree.size);
        /* South */
        neighbor = gtequal_adj_neighbor(&tree,south);
        if ((neighbor==NULL) || (neighbor->color==white)) retval += tree.size;
        else if (neighbor->color==grey) 
            retval += sum_adjacent(neighbor, northwest, northeast, tree.size);
        /* West */
        neighbor=gtequal_adj_neighbor(&tree,west);
        if ((neighbor==NULL) || (neighbor->color==white)) retval += tree.size;
        else if (neighbor->color==grey) 
            retval += sum_adjacent(neighbor, northeast, southeast, tree.size);
    }
    
    return retval;
}

extern int dealwithargs(int argc, char * argv[]);

extern bool parallel;
extern bool balanced;

int main(int argc, char *argv[])
{
    //QuadTree tree;
    int count;
    int level;

#ifdef DEBUG
    printf("main.nopf.c: sizeof(quad_struct)=%d\n", sizeof(quad_struct));
#endif

    level = dealwithargs(argc,argv);

    chatting("Perimeter with %d levels on %d processors\n",level,NumNodes);
    
    //tree=MakeTree(2048*1024,0,0,0,NumNodes-1,NULL,southeast,level); // just 2048
    Fractal_t fractal;

    if (parallel) {
        fractal.set_impl_type(Fractal_t::ImplType::parallel);
        chatting("parallel\n");
    } else {
        fractal.set_impl_type(Fractal_t::ImplType::sequential);
        chatting("sequential\n");
    }

    if (balanced) {
        fractal.set_type(Fractal_t::Type::balanced);
        chatting("balanced\n");
    } else {
        fractal.set_type(Fractal_t::Type::unbalanced);
        chatting("unbalanced\n");
    }

    Fractal_t::Seed_t growth_seed;
    growth_seed.size = 2048;
    growth_seed.center_x = 0;
    growth_seed.center_y = 0;
    growth_seed.lo_proc = 0;
    growth_seed.hi_proc = NumNodes-1;
    growth_seed.ct = southeast;

    fractal.grow(level, growth_seed);
        
#ifdef DEBUG
    //printf("After MakeTree(2048...), tree=%p *tree is: color=%d  childtype=%d nw=%p ne=%p sw=%p se=%p parent=%p\n", tree, tree->color, tree->childtype, tree->nw, tree->ne, tree->sw, tree->se, tree->parent);
#endif

    CountTree comp_func;
    count = fractal.template compute<CountTree::Compute_t>(comp_func);
    
    chatting("# of leaves is %d\n", count);

    //count=perimeter(tree,4096);
    Perimeter func;
    count = fractal.template compute<Perimeter::Compute_t>(func);

    chatting("perimeter is %d\n",count);

    exit(0);
}

