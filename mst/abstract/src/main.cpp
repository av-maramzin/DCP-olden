/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "mst.h"

typedef struct blue_return {
    Graph_t::node_iterator vert;
    int dist;
} BlueReturn;

typedef struct fc_br {
    BlueReturn value;
} future_cell_BlueReturn;

static BlueReturn BlueRule(Graph_t& graph, Graph_t::node_iterator inserted_it, Graph_t::node_iterator vlist_it)
{
    BlueReturn retval;
    Graph_t::node_iterator prev_it;
    int dist;
    int dist2;
    int count;
  
    if (vlist_it == graph.node_end()) {
        retval.dist = 999999;
        return retval;
    }

    prev_it = vlist_it;
    retval.vert = vlist_it;
    retval.dist = vlist_it->mindist;
   
    int inserted_idx = std::distance(inserted_it, graph.node_begin());
    int vlist_idx = std::distance(vlist_it, graph.node_begin());

    Edge_t& edge = graph.find_edge(std::make_pair(vlist_idx,inserted_idx));
    dist = edge.dist;
    
    /*printf("Found %d at 0x%x for 0x%x\n",dist,inserted,vlist);*/
    if (dist) { 
        if (dist < retval.dist) {
            vlist_it->mindist = dist;
            retval.dist = dist;
        }
    }
    else printf("Not found\n");
  
    count = 0;
    /* We are guaranteed that inserted is not first in list */
    for (Graph_t::node_iterator node_it = vlist_it; node_it != graph.node_end(); prev_it=node_it, node_it++) 
    {
        count++;
        if (node_it == inserted_it) { 
            continue;
        } else {
            int node_idx = std::distance(node_it, graph.node_begin());
            Edge_t& edge = graph.find_edge(std::make_pair(node_idx,inserted_idx));
            if (edge == Graph_t::invalid_edge) {
                continue;
            }
            dist2 = node_it->mindist;
            dist = edge.dist;
        
            /*printf("Found %d at 0x%x for 0x%x\n",dist,inserted,tmp);*/
            if (dist) {
                if (dist < dist2) {
                    node_it->mindist = dist;
                    dist2 = dist;
                }
            }
            else printf("Not found\n");
          
            if (dist2 < retval.dist) {
                retval.vert = node_it;
                retval.dist = dist2;
            }
        } /* else */
    } /* for */
  /*printf("Count was %d\n",count);*/
    return retval;
}

static Graph_t::node_iterator MyVertexList_it; 

static BlueReturn Do_all_BlueRule(Graph_t& graph, Graph_t::node_iterator inserted_it) {
    future_cell_BlueReturn fcleft;
    BlueReturn retright;

    if (inserted_it == MyVertexList_it) MyVertexList_it++;

    return BlueRule(graph, inserted_it, MyVertexList_it);
}

static int ComputeMst(Graph_t& graph, int numvert)
{
    int dist;
    int cost = 0;

    /* make copy of graph */
    printf("Compute phase 1\n");

    /* Insert first node */
    Graph_t::node_iterator inserted_it = graph.node_begin();
    Graph_t::node_iterator tmp_it = (graph.node_begin())++;
    MyVertexList_it = tmp_it;
 
    numvert--;
    /* Announce insertion and find next one */
    printf("Compute phase 2\n");
    while (numvert) 
    {
        BlueReturn br;
      
        br = Do_all_BlueRule(graph, inserted_it);
        inserted_it = br.vert;
        dist = br.dist;
        numvert--;
        cost = cost + dist;
    }
    
    return cost;
}

int main(int argc, char *argv[]) 
{
    int dist;
    int size;

    size = dealwithargs(argc,argv);
    printf("Making graph of size %d\n",size);
    
    Graph<Node_t,Edge_t> graph(size);

    MakeGraph(graph, size);
    printf("Graph completed\n");

    printf("About to compute mst \n");

    dist = ComputeMst(graph, size);

    printf("MST has cost %d\n",dist);
    
    exit(0);
}
