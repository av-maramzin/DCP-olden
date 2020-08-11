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
    
    sum = CheckOutside(center_x+size,center_y+size) +
          CheckOutside(center_x+size,center_y-size) +
          CheckOutside(center_x-size,center_y-size) +
	  CheckOutside(center_x-size,center_y+size);
    
    if ((sum==4) || (sum==-4)) return 0;
  
    return 1;
}  

void QuadStruct::grow(Fractal_t::Seed_t seed) {

    int intersect = 0;
    int size = seed.size;
    
    this->childtype = seed.ct;
    this->size = size;

    intersect = CheckIntersect(seed.center_x, seed.center_y, seed.size);
    size = size/2;
  
    if ((intersect==0) && (size<1)) {
        this->color = white;
    } else if (intersect==2) {
        this->color=black;
    } else {
        if (this->element_info().level == 1) {
            this->color = black;
	} else {
            this->color = grey;
	}
    }
}

Fractal_t::Seed_t QuadStruct::spawn_child_seed(int child_id) {
    
    Fractal_t::Seed_t parent_seed = this->get_seed();
    Fractal_t::Seed_t child_seed;

    child_seed.ct = (ChildType)child_id;
    int size = child_seed.size = parent_seed.size/2;

    int mid1, mid2;
    mid1 = (parent_seed.lo_proc + parent_seed.hi_proc)/2;
    mid2 = (parent_seed.lo_proc + parent_seed.hi_proc + 1)/2;

    switch (child_seed.ct) {
        case northwest:
            child_seed.center_x = parent_seed.center_x - size;
	    child_seed.center_y = parent_seed.center_y + size;
            child_seed.lo_proc = parent_seed.lo_proc;
            child_seed.hi_proc = (parent_seed.lo_proc + mid1)/2;
            break;
        case northeast:
            child_seed.center_x = parent_seed.center_x + size;
	    child_seed.center_y = parent_seed.center_y + size;
            child_seed.lo_proc = (parent_seed.lo_proc + mid1 + 1)/2;
            child_seed.hi_proc = mid1;
            break;
        case southwest:
	    child_seed.center_x = parent_seed.center_x - size;
	    child_seed.center_y = parent_seed.center_y - size;
            child_seed.lo_proc = (mid2 + parent_seed.hi_proc + 1)/2;
            child_seed.hi_proc = parent_seed.hi_proc;
            break;
        case southeast:
            child_seed.center_x = parent_seed.center_x + size;
	    child_seed.center_y = parent_seed.center_y - size;
            child_seed.lo_proc = mid2;
            child_seed.hi_proc = (mid2 + parent_seed.hi_proc)/2;;
            break;
    };

    return child_seed;
}

bool QuadStruct::growth_stop_condition() {

    int intersect = 0;
    int size = this->get_seed().size;

    intersect = CheckIntersect(this->get_seed().center_x, this->get_seed().center_y, size);

    size = size/2;
  
    if ( (intersect == 0) && (size<1) ) {
        return true;
    } else if (intersect == 2) {
        return true;
    } else {
        if (this->element_info().level == 1) {
	    return true;
        } else {
	    return false;
        }
    }
}

//
