#include "containers.h"

#define RANGE 100
extern int random(int);
extern int dealwithargs(int argc, char *argv[]);

TreeContainer::TreeContainer(int argc, char *argv[]) {
    n = dealwithargs(argc, argv);
    root = RandTree(n, 12345768, 0, 0);
    sval = random(245867) % RANGE;
}
TreeContainer::~TreeContainer() {}
        
void TreeContainer::fill(HANDLE *h, SequentialContainer& seqCont) {
    if ((h != NIL)) {
        HANDLE *l = h->left;
        HANDLE *r = h->right;
                
        if (l != NIL) {
            fill(l, seqCont);
        }
                
        seqCont.add(h->value);
                
        if (r != NIL) {
            fill(r, seqCont);
        }
    }
}
