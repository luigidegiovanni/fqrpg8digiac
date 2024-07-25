#include "Graph.h"
#include <iostream>

#include "fqrpg_instance.h"

using namespace std;

void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w); // Add w to the list of v.
}

void Graph::DFS(int v)
{
    // Mark the current node as visited and print it
    visited[v] = true;
    cout << "*" << alpha(v) << endl;

    // Recur for all the vertices adjacent to this vertex
    list<int>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i) {
      if (!visited[*i])
        DFS(*i);
      cout << "!" << alpha(v) << endl;
    }
}

extern int verbose;
/* Recursively compute the "rank" of any node v reachable from a node r
 * as the smaller number of arcs of a path from r to v 
 * (for trees, the rank is the depth of v in the tree rooted in r)
 * Call with nodeRanking(root, zero, output_int )
 */
void Graph::nodeRanking(int v, int currRank, int& maxRank )
{
    if (visited[v]) {
        throw std::runtime_error("* this is not an arborescence *");
    }
    if (maxRank < currRank) maxRank = currRank;
    visited[v] = true;
    rank[v] = currRank;
    if (verbose>=2) cout << "node " << v << " , rank " << currRank << endl;
    ++currRank;

    list<int>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
        if (!visited[*i])
            nodeRanking(*i,currRank,maxRank);
}
