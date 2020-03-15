/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "perimeter.h"
#include <stdio.h>
#include <stdlib.h>

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

int CountTree(QuadTree tree) 
{
    QuadTree nw,ne,sw,se;

    nw = tree->nw; ne = tree->ne; sw = tree->sw; se = tree->se;
    if (nw==NULL && ne==NULL && sw==NULL && se==NULL)
        return 1;
    else
        return CountTree(nw) + CountTree(ne) + CountTree(sw) +
            CountTree(se);
}

static QuadTree child(QuadTree tree, ChildType ct)
{
  switch(ct) 
    {
    case northeast:
      return tree->ne;
    case northwest:
      return tree->nw;
    case southeast:
      return tree->se;
    case southwest:
      return tree->sw;
    default:
#ifdef DEBUG
      printf("\n bug in child()\n");
      exit(1);
#endif
      return 0;
    }
}

static QuadTree gtequal_adj_neighbor(QuadTree tree, Direction d)
{
  QuadTree q,parent;
  ChildType ct;
  
    parent=tree->parent;
    ct=tree->childtype;
  if ((parent!=NULL) && adj(d,ct))
    q=gtequal_adj_neighbor(parent,d);
  else q=parent;
  if (q && q->color==grey) {
    return child(q,reflect(d,ct));
  }
  else return q;
}

static int sum_adjacent(QuadTree p, ChildType q1, ChildType q2, int size)
{
  if (p->color==grey) 
    {
      return sum_adjacent(child(p,q1),q1,q2,size/2) +
	sum_adjacent(child(p,q2),q1,q2,size/2);
    }
  else if (p->color==white) 
    {
      return size;
    }
  else return 0;
}

int perimeter(QuadTree tree, int size)
{
    int retval = 0;
    QuadTree neighbor;

    if (tree->color==grey) 
    {
        QuadTree child;

        child = tree->sw;
        retval += perimeter(child,size/2);
        child = tree->se;
        retval += perimeter(child,size/2);
        child = tree->ne;
        retval += perimeter(child,size/2);
        child = tree->nw;
        retval += perimeter(child,size/2);
    
    } else if (tree->color==black) {
        
        /* North */
        neighbor=gtequal_adj_neighbor(tree,north);
        if ((neighbor==NULL) || (neighbor->color==white)) retval+=size;
        else if (neighbor->color==grey) 
	    retval+=sum_adjacent(neighbor,southeast,southwest,size);
        /* East */
        neighbor=gtequal_adj_neighbor(tree,east);
        if ((neighbor==NULL) || (neighbor->color==white)) retval+=size;
        else if (neighbor->color==grey) 
	    retval+=sum_adjacent(neighbor,southwest,northwest,size);
        /* South */
        neighbor=gtequal_adj_neighbor(tree,south);
        if ((neighbor==NULL) || (neighbor->color==white)) retval+=size;
        else if (neighbor->color==grey) 
	    retval+=sum_adjacent(neighbor,northwest,northeast,size);
        /* West */
        neighbor=gtequal_adj_neighbor(tree,west);
        if ((neighbor==NULL) || (neighbor->color==white)) retval+=size;
        else if (neighbor->color==grey) 
	    retval+=sum_adjacent(neighbor,northeast,southeast,size);
    }
    
    return retval;
}

extern int dealwithargs(int argc, char* argv[]);

int main(int argc, char *argv[])
{
    QuadTree tree;
    int count;
    int level;

#ifdef DEBUG
    printf("main.nopf.c: sizeof(quad_struct)=%d\n", sizeof(quad_struct));
#endif

    level = dealwithargs(argc,argv);

    chatting("Perimeter with %d levels on %d processors\n",level,NumNodes);
    //tree=MakeTree(2048*1024,0,0,0,NumNodes-1,NULL,southeast,level);
    tree=MakeTree(2048,0,0,0,NumNodes-1,NULL,southeast,level);

#ifdef DEBUG
    printf("After MakeTree(2048...), tree=%p *tree is: color=%d  childtype=%d nw=%p ne=%p sw=%p se=%p parent=%p\n", tree, tree->color, tree->childtype, tree->nw, tree->ne, tree->sw, tree->se, tree->parent);
#endif

    count=CountTree(tree);
    chatting("# of leaves is %d\n",count);

  //count=perimeter(tree,4096);
  count=perimeter(tree,2048);

  chatting("perimeter is %d\n",count);

  exit(0);
}

