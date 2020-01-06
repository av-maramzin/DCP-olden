/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "perimeter.h"
#include <stdlib.h>

static int CheckOutside(int x, int y) 
{
  int euclid = x*x+y*y;

  if (euclid > 4194304) return 1;  
  if (euclid < 1048576) return -1; 
  return 0;
}

static int CheckIntersect(int center_x, int center_y, int size)
{
  int sum;
  
  if (!CheckOutside(center_x+size,center_y+size) &&
      !CheckOutside(center_x+size,center_y-size) &&
      !CheckOutside(center_x-size,center_y-size) &&
      !CheckOutside(center_x-size,center_y+size)) return 2;
  sum=CheckOutside(center_x+size,center_y+size) +
    CheckOutside(center_x+size,center_y-size) +
      CheckOutside(center_x-size,center_y-size) +
	CheckOutside(center_x-size,center_y+size);
  if ((sum==4) || (sum==-4)) return 0;
  return 1;
}  

//QuadTree MakeTree(int size, int center_x, int center_y, int lo_proc,
//		  int hi_proc, QuadTree parent, ChildType ct, int level)
void growth_func(FractalElementData_t* elem, const FractalElementInfo& info, GrowthSeed_t seed) 
{
    int intersect = 0;
    int size = seed.size;
    //QuadTree retval;

    //retval = (QuadTree) malloc(sizeof(*retval));
    //retval->parent = parent;
    //retval->childtype = ct;
    
    elem->childtype = seed.ct;
    elem->size = size;

    intersect = CheckIntersect(seed.center_x, seed.center_y, seed.size);
    size = size/2;
/*
    std::string ct_str;
    switch (seed.ct) {
        case northwest:
            ct_str = "northwest";
            break;
        case northeast:
            ct_str = "northeast";
            break;
        case southwest:
            ct_str = "southwest";
            break;
        case southeast:
            ct_str = "southeast";
            break;
        default:
            ct_str = "none";
            break;
    }*/
  
    if ((intersect==0) && (size<1)) {
//        std::cout << "FractalElement(level=" << info.level << ", size=" << elem->size << ", color=white, ct=" << ct_str << std::endl;
        elem->color = white;
    } else if (intersect==2) {
//        std::cout << "FractalElement(level=" << info.level << ", size=" << elem->size << ", color=black, ct=" << ct_str << std::endl;
        elem->color=black;
    } else {
        if (!info.level) {
//            std::cout << "FractalElement(level=" << info.level << ", size=" << elem->size << ", color=black, ct=" << ct_str << std::endl;
            elem->color = black;
	} else {
//            std::cout << "FractalElement(level=" << info.level << ", size=" << elem->size << ", color=grey, ct=" << ct_str << std::endl;
            elem->color = grey;
	}
    }
}

//QuadTree MakeTree(int size, int center_x, int center_y, int lo_proc,
//		  int hi_proc, QuadTree parent, ChildType ct, int level) 
void next_growth_seed_func(const GrowthSeed_t& parent_seed, GrowthSeed_t& next_child_seed, int next_child_num)
{
    next_child_seed.ct = (ChildType)next_child_num;
    int size = next_child_seed.size = parent_seed.size/2;

    int mid1, mid2;
    mid1 = (parent_seed.lo_proc + parent_seed.hi_proc)/2;
    mid2 = (parent_seed.lo_proc + parent_seed.hi_proc + 1)/2;

    switch (next_child_seed.ct) {
        case northwest:
            next_child_seed.center_x = parent_seed.center_x - size;
	    next_child_seed.center_y = parent_seed.center_y + size;
            next_child_seed.lo_proc = parent_seed.lo_proc;
            next_child_seed.hi_proc = (parent_seed.lo_proc + mid1)/2;
            break;
        case northeast:
            next_child_seed.center_x = parent_seed.center_x + size;
	    next_child_seed.center_y = parent_seed.center_y + size;
            next_child_seed.lo_proc = (parent_seed.lo_proc + mid1 + 1)/2;
            next_child_seed.hi_proc = mid1;
            break;
        case southwest:
	    next_child_seed.center_x = parent_seed.center_x - size;
	    next_child_seed.center_y = parent_seed.center_y - size;
            next_child_seed.lo_proc = (mid2 + parent_seed.hi_proc + 1)/2;
            next_child_seed.hi_proc = parent_seed.hi_proc;
            break;
        case southeast:
            next_child_seed.center_x = parent_seed.center_x + size;
	    next_child_seed.center_y = parent_seed.center_y - size;
            next_child_seed.lo_proc = mid2;
            next_child_seed.hi_proc = (mid2 + parent_seed.hi_proc)/2;;
            break;
    };
}

//QuadTree MakeTree(int size, int center_x, int center_y, int lo_proc,
//		  int hi_proc, QuadTree parent, ChildType ct, int level)
bool growth_stop_func(const FractalElementInfo& info, const GrowthSeed_t& growth_seed)
{
    int intersect = 0;
    int size = growth_seed.size; 

    intersect = CheckIntersect(growth_seed.center_x, growth_seed.center_y, size);

    size = size/2;
  
    if ( (intersect == 0) && (size<1) ) {
        return true;
    } else if (intersect == 2) {
        return true;
    } else {
        if (!info.level) {
	    return true;
        } else {
	    return false;
        }
    }
}

/*
QuadTree MakeTree(int size, int center_x, int center_y, int lo_proc,
		  int hi_proc, QuadTree parent, ChildType ct, int level) 
{
  int intersect=0;
  QuadTree retval;

  retval = (QuadTree) malloc(sizeof(*retval));
  retval->parent = parent;
  retval->childtype = ct;

  intersect = CheckIntersect(center_x,center_y,size);
  size = size/2;
  if ((intersect==0) && (size<512))
      {
      retval->color = white;
      retval->nw = NULL;
      retval->ne = NULL;
      retval->sw = NULL;
      retval->se = NULL;
    }
  else if (intersect==2) 
    {
      retval->color=black;
      retval->nw = NULL;
      retval->ne = NULL;
      retval->sw = NULL;
      retval->se = NULL;
    }
  else 
    {
      if (!level)
	{
	  retval->color = black;
	  retval->nw = NULL;
	  retval->ne = NULL;
	  retval->sw = NULL;
	  retval->se = NULL;
	}
      else 
	{
	  int mid1,mid2;

	  mid1 = (lo_proc+hi_proc)/2;
	  mid2 = (lo_proc+hi_proc+1)/2;

	  retval->sw = MakeTree(size,center_x-size,center_y-size,
				(mid2+hi_proc+1)/2,hi_proc,retval,
				southwest,level-1);
	  retval->se = MakeTree(size,center_x+size,center_y-size,
				mid2,(mid2+hi_proc)/2,retval,
				southeast,level-1);
	  retval->ne = MakeTree(size,center_x+size,center_y+size,
				(lo_proc+mid1+1)/2,mid1,retval,
				northeast,level-1);
	  retval->nw = MakeTree(size,center_x-size,center_y+size,
				lo_proc,(lo_proc+mid1)/2,retval,
				northwest,level-1);
	  retval->color = grey;
	}
    }
  return retval;
}*/

//
