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
      return ((ct==northeast) || (ct==northwest));
    case south:
      return ((ct==southeast) || (ct==southwest));
    case east:
      return ((ct==northeast) || (ct==southeast));
    case west:
      return ((ct==southwest) || (ct==northwest));
    }
}

static ChildType reflect(Direction d, ChildType ct) 
{
  if ((d==west) || (d==east)) 
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

FractalApply_CountTree_ret_t CountTree(FractalElementData_t* tree, const std::vector<FractalApply_CountTree_ret_t>& child_rets)
{
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

static FractalElement_t* child(FractalElement_t* tree, ChildType ct)
{
    return tree->get_child_ptr((int)ct);
}

static FractalElement_t* gtequal_adj_neighbor(FractalElement_t* tree, Direction d)
{
    FractalElement_t* q;
    FractalElement_t* parent;
    ChildType ct;
  
    parent = tree->get_parent_ptr();
    ct = tree->get_elem_data()->childtype;

    if ((parent!=NULL) && adj(d,ct))
        q=gtequal_adj_neighbor(parent,d);
    else q=parent;
    if (q && q->get_elem_data()->color==grey) {
        return child(q,reflect(d,ct));
    }
    else return q;
}

static int sum_adjacent(FractalElement_t* p, ChildType q1, ChildType q2, int size)
{
    if (p->get_elem_data()->color == grey) 
    {
        return sum_adjacent(child(p,q1),q1,q2,size/2) +
	    sum_adjacent(child(p,q2),q1,q2,size/2);
    } 
    else if (p->get_elem_data()->color == white) 
    {
        return size;
    } 
    else return 0;
}

//int perimeter(QuadTree tree, int size)
FractalWalk_perimeter_ret_t perimeter(FractalElement_t* tree, const std::vector<FractalWalk_perimeter_ret_t>& child_rets)
{
    int retval = 0;
    FractalElement_t* neighbor;

    if (tree->get_elem_data()->color == grey) {

        if (!tree->has_children()) {
            std::cerr << "FractalWalk::perimeter::grey node must have children!" << std::endl; 
            std::exit(EXIT_FAILURE);
        }

        for (auto child_ret : child_rets) {
            retval += child_ret;
        }
    } else if (tree->get_elem_data()->color == black) {

        if (tree->has_children()) {
            std::cerr << "FractalWalk::perimeter::black node cannot have any children!" << std::endl; 
            std::exit(EXIT_FAILURE);
        }

        /* North */
        neighbor=gtequal_adj_neighbor(tree,north);
        if ((neighbor==NULL) || (neighbor->get_elem_data()->color==white)) retval += tree->get_elem_data()->size;
        else if (neighbor->get_elem_data()->color==grey) 
	    retval+=sum_adjacent(neighbor,southeast,southwest,tree->get_elem_data()->size);
        /* East */
        neighbor=gtequal_adj_neighbor(tree,east);
        if ((neighbor==NULL) || (neighbor->get_elem_data()->color==white)) retval+=tree->get_elem_data()->size;
        else if (neighbor->get_elem_data()->color==grey) 
	    retval+=sum_adjacent(neighbor,southwest,northwest,tree->get_elem_data()->size);
        /* South */
        neighbor=gtequal_adj_neighbor(tree,south);
        if ((neighbor==NULL) || (neighbor->get_elem_data()->color==white)) retval+=tree->get_elem_data()->size;
        else if (neighbor->get_elem_data()->color==grey) 
            retval+=sum_adjacent(neighbor,northwest,northeast,tree->get_elem_data()->size);
        /* West */
        neighbor=gtequal_adj_neighbor(tree,west);
        if ((neighbor==NULL) || (neighbor->get_elem_data()->color==white)) retval+=tree->get_elem_data()->size;
        else if (neighbor->get_elem_data()->color==grey) 
            retval+=sum_adjacent(neighbor,northeast,southeast,tree->get_elem_data()->size);
    }
    
    return retval;
}

extern int dealwithargs(int argc, char * argv[]);

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
    Fractal<FractalElementData_t,4> fractal;
    
    GrowthSeed_t growth_seed;
    
    growth_seed.size = 2048;
    growth_seed.center_x = 0;
    growth_seed.center_y = 0;
    growth_seed.lo_proc = 0;
    growth_seed.hi_proc = NumNodes-1;
    growth_seed.ct = southeast;

    fractal.template grow<GrowthFunc_t,
                          GrowthSeed_t,
                          NextGrowthSeedFunc_t,
                          GrowthStopFunc_t>(level,
                                            growth_func, 
                                            growth_seed, 
                                            next_growth_seed_func,
                                            growth_stop_func);
#ifdef DEBUG
    printf("After MakeTree(2048...), tree=%p *tree is: color=%d  childtype=%d nw=%p ne=%p sw=%p se=%p parent=%p\n", tree, tree->color, tree->childtype, tree->nw, tree->ne, tree->sw, tree->se, tree->parent);
#endif

    //count=CountTree(tree);
    count = fractal.template apply<FractalApply_CountTree_func_t,FractalApply_CountTree_ret_t>(CountTree);
    
    chatting("# of leaves is %d\n",count);

    //count=perimeter(tree,4096);
    count = fractal.template walk<FractalWalk_perimeter_func_t,FractalWalk_perimeter_ret_t>(perimeter);

    chatting("perimeter is %d\n",count);

    exit(0);
}

