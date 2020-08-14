/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "perimeter.h"
#include <stdlib.h>

static int CheckOutside(long long int x, long long int y) 
{
  unsigned long long int euclid = x*x+y*y;

  if (euclid > 4294967296) return 1;  
  if (euclid < 1073741824) return -1; 
 
  return 0;
}

static int CheckIntersect(long long int center_x, long long int center_y, long long int size)
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

QuadTree MakeTree(long long int size, long long int center_x, long long int center_y, int lo_proc,
		  int hi_proc, QuadTree parent, ChildType ct, int level) 
{
  int intersect=0;
  QuadTree retval;

#ifdef FUTURES
  retval = (QuadTree) ALLOC(lo_proc,sizeof(*retval));
#else
  retval = (QuadTree) malloc(sizeof(*retval));
#endif
  retval->parent = parent;
  retval->childtype = ct;

  /*
    char* ct_str = NULL;
    switch (ct) {
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

  intersect = CheckIntersect(center_x,center_y,size);
  size = size/2;
  if ((intersect==0) && (size<1))
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
#ifdef FUTURES 
     future_cell_int fc_sw,fc_se,fc_ne;
#endif

	  mid1 = (lo_proc+hi_proc)/2;
	  mid2 = (lo_proc+hi_proc+1)/2;

#ifndef FUTURES
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
#else
	  FUTURE(size,center_x-size,center_y-size,
				(mid2+hi_proc+1)/2,hi_proc,retval,
				southwest,level-1,MakeTree,&fc_sw);
	  FUTURE(size,center_x+size,center_y-size,
				mid2,(mid2+hi_proc)/2,retval,
				southeast,level-1,MakeTree,&fc_se);
	  FUTURE(size,center_x+size,center_y+size,
				(lo_proc+mid1+1)/2,mid1,retval,
				northeast,level-1,MakeTree,&fc_ne);
	  retval->nw = MakeTree(size,center_x-size,center_y+size,
				lo_proc,(lo_proc+mid1)/2,retval,
				northwest,level-1);
     TOUCH(&fc_sw);
     retval->sw = (QuadTree) fc_sw.value;
     TOUCH(&fc_se);
     retval->se = (QuadTree) fc_se.value;
     TOUCH(&fc_ne);
     retval->ne = (QuadTree) fc_ne.value;
#endif
	  retval->color = grey;
	}
    }
  /*
    if (retval->color == white) {
        printf("FractalElement(level=%d, size=%d, color=white, ct=%s\n",level,size,ct_str);
    } else if (retval->color == black) {
        printf("FractalElement(level=%d, size=%d, color=black, ct=%s\n",level,size,ct_str);
    } else {
        printf("FractalElement(level=%d, size=%d, color=grey, ct=%s\n",level,size,ct_str);
    }*/
  return retval;
}



