/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "mst.h"

/*#define assert(num,a) \
   if (!(a)) {printf("Assertion failure:%d in makegraph\n",num); exit(-1);}*/

#define CONST_m1 10000
#define CONST_b 31415821
#define RANGE 2048

static int mult(int p, int q)
{
    int p1, p0, q1, q0;

    p1=p/CONST_m1; p0=p%CONST_m1;
    q1=q/CONST_m1; q0=q%CONST_m1;
    
    return (((p0*q1+p1*q0) % CONST_m1)*CONST_m1+p0*q0);
}

static int random2(int seed)
{
    int tmp;
    tmp = (mult(seed,CONST_b)+1);
    return tmp;
}

static int compute_dist(int i,int j, int numvert)
{
    int less, gt;
    if (i<j) {less = i; gt = j;} else {less = j; gt = i;}
    return (random2(less*numvert+gt) % RANGE)+1;
}

static void AddEdges(Graph_t& graph) 
{
    for (typename Graph_t::node_iterator node_it = graph.node_begin();
         node_it != graph.node_end();
         node_it++) 
    {
        int from_node_index = std::distance(graph.node_begin(), node_it);

        for (int to_node_index = 0; 
             to_node_index < graph.node_size(); 
             to_node_index++)
        {
            if (to_node_index != from_node_index) // no self-cycles 
            {
                Edge_t edge;
                edge.dist = compute_dist(to_node_index, from_node_index, graph.node_size());
                graph.add_edge(std::make_pair(from_node_index, to_node_index), edge);
            }
        }
    }
}

void MakeGraph(Graph_t& graph, int numvert)
{
    chatting("Make phase 2\n");
    
    for (int i = 0; i < numvert; i++) {
        Node_t vertex;
        vertex.mindist = 9999999;
        
        graph.add_node(i,vertex);
    }

    chatting("Make phase 3\n");
    
    AddEdges(graph);
    
    chatting("Make phase 4\n");

    chatting("Make returning\n");
}

//
