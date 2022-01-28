// AED 2021/2022 - Aula Pratica 09
// Pedro Ribeiro (DCC/FCUP) [03/01/2022]

#include "graph.h"
#include "minHeap.h"
#include <vector>
#define INF (INT_MAX/2)

// Constructor: nr nodes and direction (default: undirected)
Graph::Graph(int num, bool dir) : n(num), hasDir(dir), nodes(num+1) {}

Graph::Graph() {}

/**
 * Function that calculates the number of connected components in the graph
 * @return number of connected components in the graph
 */
int Graph::connectedComponents() {
    int counter=0;
    for (int i=1;i<=n;i++)
        nodes[i].visited = false;
    for (int i=1;i<=n;i++)
        if (!nodes[i].visited)
        {
            counter++;
            dfs(i, 0, vector<int>());
        }
    return counter;
}

/**
 * Function that adds an edge between two nodes
 * @param src first node
 * @param dest second node
 * @param line the code of the bus' line that connects the two nodes
 * @param d distance between the nodes (bus stops)
 * @param foot a boolean to flag the links that aren't possible using buses
 * @param weight an integer to represent the amount of zone transitions made by the edge
 */
void Graph::addEdge(int src, int dest, string line, double d, bool foot, int weight) {
    line = line.substr(0, line.find('.'));
    if (src<1 || src>n || dest<1 || dest>n)
        return;
    nodes[src].adj.push_back({dest, weight, std::move(line), d, foot});
    if (!hasDir)
        nodes[dest].adj.push_back({src, weight});
}

int Graph::sameLine(int v) {
    for (auto e:nodes[v].adj) {
        if (e.line == nodes[v].edgePrev.line)
            return e.dest;
    }
    return -1;
}

void Graph::rearrangeList(int v){
    list<Edge> tmp;
    for (auto it=nodes[v].adj.begin();it!=nodes[v].adj.end();it++) {
        if (it->line == nodes[v].edgePrev.line)
            tmp.push_front(*it);
        else{
            tmp.push_back(*it);
        }
    }
    nodes[v].adj = tmp;
}

void printPath(vector<int> stack)
{
    int i;
    for (i = 0; i < (int)stack.size() - 1; i++) {
        cout << stack[i] << " -> ";
    }
    cout << stack[i];
}

/**
 * Standard depth first search
 * @param start starting node
 */
vector<int> Graph::dfs(int start, int target, vector<int> stack) {
    // cout << start << " "; // show node order
    stack.push_back(start);
    if (start == target) {
        return stack;
    }
    nodes[start].visited = true;
    if (!nodes[start].adj.empty()){
        rearrangeList(start);
        for (const auto &edge: nodes[start].adj) {
            if (!nodes[edge.dest].visited)
                dfs(edge.dest, target, stack);
        }
    }
    if (!stack.empty())
        stack.pop_back();
}


/**
 * Standard breath first search
 * @param v starting node
 */
void Graph::bfs(int v) {
    for (int v=1; v<=n; v++) {
        nodes[v].visited = false;
        nodes[v].pred = 0;
        nodes[v].dist = 0;
    }
    queue<int> q; // queue of unvisited nodes
    q.push(v);
    nodes[v].visited = true;
    nodes[v].pred = v;
    while (!q.empty()) { // while there are still unvisited nodes
        int u = q.front(); q.pop();
        // cout << u << " "; // show node order
        for (const auto& e : nodes[u].adj) {
            int w = e.dest;
            if (!nodes[w].visited) {
                q.push(w);
                nodes[w].visited = true;
                nodes[w].pred = u;
                nodes[w].edgePrev = e;
                nodes[w].dist = nodes[u].dist+1;
            }
        }
    }
}

 /**
  * Dijkstra algorithm that calculates the distance between two nodes, while making it possible to reconstruct the path
  * @param a starting node
  * @param b target node
  * @return the distance (integer) between the nodes
  */
int Graph::dijkstra_distance(int a, int b) {
    MinHeap<int, int> q(n, -1);
    for (int i=1;i<=n;i++){
        if (i==a) {
            nodes[a].dist = 0;
            nodes[a].pred = a;
        }
        else {
            nodes[i].dist = INF;
            nodes[i].visited = false;
            nodes[i].pred = 0;
        }
        q.insert(i, nodes[i].dist);
    }
    while (q.getSize() > 0){
        int u = q.removeMin();
        nodes[u].visited = true;
        for (const auto& v:nodes[u].adj){
            if (!nodes[v.dest].visited && nodes[u].dist + v.zoneChanges < nodes[v.dest].dist){
                nodes[v.dest].dist = nodes[u].dist + v.zoneChanges;
                nodes[v.dest].pred = u;
                q.decreaseKey(v.dest, nodes[v.dest].dist);
            }
        }
    }
    if (nodes[b].dist == INF)
        return -1;
    else
        return nodes[b].dist;
}

/**
 * Dijkstra algorithm that reconstructs the path used by the dijkstra algorithm implementation that calculates the distance between two nodes
 * @param a starting node
 * @param b target node
 * @return a list with the integers that are associated with each node of the path
 */
list<int> Graph::dijkstra_path(int a, int b) {
    list<int> path;
    dijkstra_distance(a, b);
    if (nodes[b].dist == INF)
        return path;
    int current = b;
    while(current != a){
        path.push_front(current);
        current = nodes[current].pred;
    }
    path.push_front(a);
    return path;
}

/**
 * Algorithm to calculate the path that contains the minimum number of stops between two nodes
 * @param a starting node
 * @param b target node
 * @return a list with the integers that are associated with each node of the path
 */
list<Edge> Graph::minimumStops(int a, int b) {
    list<Edge> res;
    int distance = dijkstra_distance(a, b);
    if (distance == -1)
        return res;
    bfs(a);
    int current = b;
    while(current != a) {
        res.push_front(nodes[current].edgePrev);
        current = nodes[current].pred;
    }
    return res;
}

/**
 * Dijkstra implementation to calculate the distance between two nodes, using a path that minimizes the amount of times that the user needs to change line
 * @param a starting node
 * @param b target node
 * @return the distance (integer) between the two nodes
 */
int Graph::dijkstra_lineChange(int a, int b) {
    MinHeap<int, int> q(n, -1);
    for (int i=1;i<=n;i++){
        if (i==a) {
            nodes[a].dist = 0;
            nodes[a].pred = a;
        }
        else {
            nodes[i].dist = INF;
            nodes[i].visited = false;
            nodes[i].pred = 0;
        }
        q.insert(i, nodes[i].dist);
    }
    while (q.getSize() > 0){
        int u = q.removeMin();
        nodes[u].visited = true;
        string prevLine;
        for (const auto& v:nodes[u].adj){
            if (!nodes[v.dest].visited && nodes[u].dist + (nodes[u].edgePrev.line == v.line ? 0 : 1) < nodes[v.dest].dist){
                nodes[v.dest].dist = nodes[u].dist + (nodes[u].edgePrev.line == v.line ? 0 : 1);
                nodes[v.dest].pred = u;
                nodes[v.dest].edgePrev = v;
                q.decreaseKey(v.dest, nodes[v.dest].dist);
            }
        }
    }
    if (nodes[b].dist == INF)
        return -1;
    else
        return nodes[b].dist;
}

/**
 * Algorithm that reconstructs the path created by the Dijkstra implementation that minimizes line changes
 * @param a starting node
 * @param b target node
 * @return a list with the integers that are associated with each node of the path
 */
list<Edge> Graph::minimumLines(int a, int b) {
    list<Edge> res;
    int distance = dijkstra_lineChange(a, b);
    if (distance == -1)
        return res;
    int current = b;
    while(current != a){
        res.push_front(nodes[current].edgePrev);
        current = nodes[current].pred;
    }
    return res;
}

/**
 * Dijkstra implementation to calculate the distance between two nodes, using a path that minimizes the distance travelled
 * @param a starting node
 * @param b target node
 * @return the distance (integer) between the two nodes
 */
int Graph::dijkstra_distanceMinDistance(int a, int b) {
    MinHeap<int, int> q(n, -1);
    for (int i=1;i<=n;i++){
        if (i==a) {
            nodes[a].dist = 0;
            nodes[a].pred = a;
        }
        else {
            nodes[i].dist = INF;
            nodes[i].visited = false;
            nodes[i].pred = 0;
        }
        q.insert(i, nodes[i].dist);
    }
    while (q.getSize() > 0){
        int u = q.removeMin();
        nodes[u].visited = true;
        for (const auto& v:nodes[u].adj){
            if (!nodes[v.dest].visited && nodes[u].dist + v.distanceRealWorld < nodes[v.dest].dist){
                nodes[v.dest].dist = nodes[u].dist + v.distanceRealWorld;
                nodes[v.dest].pred = u;
                nodes[v.dest].edgePrev = v;
                q.decreaseKey(v.dest, nodes[v.dest].dist);
            }
        }
    }
    if (nodes[b].dist == INF)
        return -1;
    else
        return nodes[b].dist;
}

/**
 * Algorithm that reconstructs the path created by the Dijkstra implementation that minimizes the distance travelled
 * @param a starting node
 * @param b target node
 * @return a list with the integers that are associated with each node of the path
 */
list<Edge> Graph::dijkstra_pathMinDistance(int a, int b) {
    list<Edge> path;
    dijkstra_distanceMinDistance(a, b);
    if (nodes[b].dist == INF)
        return path;
    int current = b;
    while(current != a) {
        path.push_front(nodes[current].edgePrev);
        current = nodes[current].pred;
    }
    return path;
}

Edge Graph::edgeBetween(int &a, int &b)
{
    for (auto &v : nodes[a].adj)
    {
        if (v.dest == b)
        {
            return v;
        }
    }
    return {};
}

/**
 * Dijkstra implementation to calculate the distance between two nodes, using a path that minimizes the zones used
 * @param a starting node
 * @param b target node
 * @return the distance (integer) between the two nodes
 */
int Graph::dijkstra_distanceMinZones(int a, int b) {
    MinHeap<int, int> q(n, -1);
    for (int i=1;i<=n;i++){
        if (i==a) {
            nodes[a].dist = 0;
            nodes[a].pred = a;
        }
        else {
            nodes[i].dist = INF;
            nodes[i].visited = false;
            nodes[i].pred = 0;
        }
        q.insert(i, nodes[i].dist);
    }
    while (q.getSize() > 0){
        int u = q.removeMin();
        nodes[u].visited = true;
        for (const auto& v:nodes[u].adj){
            if (!nodes[v.dest].visited && nodes[u].dist + v.zoneChanges < nodes[v.dest].dist){
                nodes[v.dest].dist = nodes[u].dist + v.zoneChanges;
                nodes[v.dest].pred = u;
                nodes[v.dest].edgePrev = v;
                q.decreaseKey(v.dest, nodes[v.dest].dist);
            }
        }
    }
    if (nodes[b].dist == INF)
        return -1;
    else
        return nodes[b].dist;
}

/**
 * Algorithm that reconstructs the path created by the Dijkstra implementation that minimizes the zones used
 * @param a starting node
 * @param b target node
 * @return a list with the integers that are associated with each node of the path
 */
list<Edge> Graph::dijkstra_pathMinZones(int a, int b) {
    list<Edge> path;
    dijkstra_distanceMinZones(a, b);
    if (nodes[b].dist == INF)
        return path;
    int current = b;
    while(current != a) {
        path.push_front(nodes[current].edgePrev);
        current = nodes[current].pred;
    }
    return path;
}

