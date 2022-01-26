// AED 2021/2022 - Aula Pratica 09
// Pedro Ribeiro (DCC/FCUP) [03/01/2022]

#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <list>
#include <queue>
#include <iostream>

using namespace std;

struct Edge {
    int dest;   // Destination node
    int weight; // An integer weight
    string line;
    double distanceRealWorld;
    bool onFoot;
};

struct Node {
    list<Edge> adj; // The list of outgoing edges (to adjacent nodes)
    bool visited;   // As the node been visited on a search?
    int dist;
    int pred;
};

class Graph {
private:
    int n;              // Graph size (vertices are numbered from 1 to n)
    bool hasDir;        // false: undirect; true: directed
    vector<Node> nodes; // The list of nodes being represented

public:
    // Constructor: nr nodes and direction (default: undirected)
    Graph(int nodes, bool dir = false);

    Graph();

    // Add edge from source to destination with a certain weight
    void addEdge(int src, int dest, string line, double d, bool foot, int weight = 1);

    // Depth-First Search: example implementation
    void dfs(int v);

    // Breadth-First Search: example implementation
    void bfs(int v);

    int dijkstra_distance(int a, int b);

    list<int> dijkstra_path(int a, int b);

    int dijkstra_lineChange(int a, int b);

    list<int> minimumStops(int a, int b);

    list<int> minimumLines(int a, int b);

    int connectedComponents();

    const vector<Node> &getNodes() const;

    list<int> minimumDistance(int a, int b);

    list<int> dijkstra_pathMinDistance(int a, int b);

    int dijkstra_distanceMinDistance(int a, int b);

    Edge edgeBetween(int &a, int &b);
};

#endif
