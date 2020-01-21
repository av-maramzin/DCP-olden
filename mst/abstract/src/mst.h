/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <stdlib.h>

#define chatting printf

#include "Graph.h"

using namespace abstract;

struct vert_st {
    vert_st() : mindist(-1) {}
    int mindist;
};
using Node_t = struct vert_st;

struct edge_st {
    edge_st() : dist(-1) {}
    int dist;
};
using Edge_t = struct edge_st;

using Graph_t = Graph<Node_t,Edge_t>;

void MakeGraph(Graph_t&, int);
int dealwithargs(int argc, char *argv[]);
