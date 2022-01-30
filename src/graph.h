#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <list>
#include <queue>
#include <iostream>
#include <utility>

using namespace std;

struct Edge {
    int dest;   // Destination node
    int zoneChanges;
    string line;
    double distanceRealWorld;
    bool onFoot;
};

struct Node {
    list<Edge> adj; // The list of outgoing edges (to adjacent nodes)
    bool visited;   // As the node been visited on a search?
    double dist;
    int pred;
    Edge edgePrev;
};

class Graph {
private:
    int n;              // Graph size (vertices are numbered from 1 to n)
    bool hasDir;        // false: undirect; true: directed
    vector<Node> nodes; // The list of nodes being represented
    list<string> forbiddenLines;


public:
    /**
    * Constructor: nr nodes and direction (default: undirected) and a list of forbidden lines
    * @param num the number of nodes
    * @param forbiddenLines the list of lines that cannot exist in the graph
    * @param dir if the graph is directed or not (default = undirected)
    */
    Graph(int nodes, list<string> forbiddenLines,bool dir = false);

    Graph();

    /**
    * Adds an edge between two nodes
    * @param src first node
    * @param dest second node
    * @param line the code of the bus line that connects the two nodes (empty if there id no bus line)
    * @param d distance between the nodes (bus stops)
    * @param foot a boolean to flag the links that aren't possible using buses
    * @param weight an integer to represent the amount of zone transitions made by the edge
    */
    void addEdge(int src, int dest, string line, double d, bool foot, int weight);

    /**
    * Standard depth first search
    * @param v starting node
    */
    void dfs(int v);

    /**
    * Standard breath first search
    * @param v starting node
    */
    void bfs(int v);

    /**
    * Dijkstra algorithm that calculates the distance between two nodes, while making it possible to reconstruct the path
    * @param a starting node
    * @param b target node
    * @return the distance (integer) between the nodes
    */
    int dijkstra_distance(int a, int b);

    /**
    * Dijkstra implementation to calculate the distance between two nodes, using a path that minimizes the amount of times that the user needs to change line
    * @param a starting node
    * @param b target node
    * @return the distance (integer) between the two nodes
    */
    int dijkstra_lineChange(int a, int b);

    /**
    * Algorithm to calculate the path that contains the minimum number of stops between two nodes
    * @param a starting node
    * @param b target node
    * @return a list with the edges that are associated to each portion of the path
    */
    list<Edge> minimumStops(int a, int b);

    /**
    * Algorithm that reconstructs the path created by the Dijkstra implementation that minimizes line changes
    * @param a starting node
    * @param b target node
    * @return a list with the edges that are associated to each portion of the path
    */
    list<Edge> minimumLines(int a, int b);

    /**
    * Calculates the amount of connected components in the graph
    * @return number of connected components in the graph
    */
    int connectedComponents();

    /**
    * Algorithm that reconstructs the path created by the Dijkstra implementation that minimizes the distance travelled
    * @param a starting node
    * @param b target node
    * @return a list with the edges that are associated to each portion of the path
    */
    list<Edge> dijkstra_pathMinDistance(int a, int b);

    /**
    * Dijkstra implementation to calculate the distance between two nodes, using a path that minimizes the distance travelled
    * @param a starting node
    * @param b target node
    * @return the distance (integer) between the two nodes
    */
    int dijkstra_distanceMinDistance(int a, int b);

    /**
    * Dijkstra implementation to calculate the distance between two nodes, using a path that minimizes the zones used
    * @param a starting node
    * @param b target node
    * @return the distance (integer) between the two nodes
    */
    int dijkstra_distanceMinZones(int a, int b);

    /**
    * Algorithm that reconstructs the path created by the Dijkstra implementation that minimizes the zones used
    * @param a starting node
    * @param b target node
    * @return a list with the edges that are associated to each portion of the path
    */
    list <Edge> dijkstra_pathMinZones(int a, int b);

    /**
     * Standard Prim's algorithm to find the MST of a graph
     * @param r starting node
     * @return pair where first element is the weight of the MST and the second is the MST
     */
    pair<double, vector<int>> prim(int r);

    /**
    * Checks if the line provided as parameter can be used
    * @param line the line (string) to be checked
    * @return true if the line isn't the "forbiddenLines" list
    */
    bool canUse(const string& line);
};

#endif
