/* For copyright information, see olden_v1.0/COPYRIGHT */

extern int NumNodes;
#define chatting      printf

#define NULL 0

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

