/* For copyright information, see olden_v1.0/COPYRIGHT */

#ifndef PERIMETER_H
#define PERIMETER_H

extern int NumNodes;

typedef enum {black, white, grey} Color;
typedef enum {northwest, northeast, southwest, southeast} ChildType;
typedef enum {north, east, south, west} Direction;

typedef struct quad_struct {
  Color color;
  ChildType childtype;

  struct quad_struct *nw;
  struct quad_struct *ne;
  struct quad_struct *sw;
  struct quad_struct *se;
  struct quad_struct *parent;

} quad_struct, *QuadTree;


QuadTree MakeTree(int size, int center_x, int center_y, int lo_proc,
                  int hi_proc, QuadTree parent, ChildType ct, int level);
int CountTree(QuadTree tree);
int perimeter(QuadTree tree, int size);

#endif // #ifndef PERIMETER_H
