/* For copyright information, see olden_v1.0/COPYRIGHT */

/* =============== NODE STRUCTURE =================== */

#ifndef TREE_SORT_H
#define TREE_SORT_H

struct node { 
  int value;
  struct node *left;
  struct node *right;
};

typedef struct node HANDLE;

typedef struct future_cell_int{
  HANDLE *value;
} future_cell_int;

extern void *malloc(unsigned);

#define NIL ((HANDLE *) 0)

HANDLE* RandTree(int n, int seed, int node, int level);
int random(int seed);
void InOrder(HANDLE *h);
int Bisort(HANDLE *root, int spr_val, int dir);
int dealwithargs(int argc, char *argv[]);

extern int NumNodes,NDim;
extern int flag;

#endif // #ifndef TREE_SORT_H
