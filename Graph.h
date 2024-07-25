#pragma once
#include <map>
#include <list>
using namespace std;

// Graph class represents a directed graph
// using adjacency list representation
class Graph
{
public:
    map<int, list<int>> adj;
    map<int, bool> visited;

    // function to add an edge to graph
    void addEdge(int v, int w);

    // DFS traversal of the vertices
    // reachable from v
    void DFS(int v);

    map<int, int> rank;
    void nodeRanking(int start, int currRank, int& maxRank);

    void clear(void) { 
      map<int, list<int>>::iterator nit;
      for (nit = adj.begin(); nit != adj.end(); ++nit) {
        nit->second.clear();
      }
      adj.clear(); visited.clear(); rank.clear(); 
    }
};

