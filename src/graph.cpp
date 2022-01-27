// AED 2021/2022 - Aula Pratica 09
// Pedro Ribeiro (DCC/FCUP) [03/01/2022]

#include "graph.h"
#include "minHeap.h"
#define INF (INT_MAX/2)

// Constructor: nr nodes and direction (default: undirected)
Graph::Graph(int num, bool dir) : n(num), hasDir(dir), nodes(num+1) {}

Graph::Graph() {}

int Graph::connectedComponents() {
    int counter=0;
    for (int i=1;i<=n;i++)
        nodes[i].visited = false;
    for (int i=1;i<=n;i++)
        if (!nodes[i].visited)
        {
            counter++;
            dfs(i);
        }
    return counter;
}

// Add edge from source to destination with a certain zoneChanges
void Graph::addEdge(int src, int dest, string line, double d, bool foot, int weight) {
    if (src<1 || src>n || dest<1 || dest>n)
        return;
    nodes[src].adj.push_back({dest, weight, std::move(line), d, foot});
    if (!hasDir)
        nodes[dest].adj.push_back({src, weight});
}


void Graph::dfs(int v) {
    // cout << v << " "; // show node order
    nodes[v].visited = true;
    for (const auto& e : nodes[v].adj) {
        int w = e.dest;
        if (!nodes[w].visited)
            dfs(w);
    }
}


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
                nodes[w].dist = nodes[u].dist+1;
            }
        }
    }
}

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

list<int> Graph::minimumStops(int a, int b) {
    list<int> res;
    int distance = dijkstra_distance(a, b);
    if (distance == -1)
        return res;
    bfs(a);
    int current = b;
    while(current != a) {
        res.push_front(current);
        current = nodes[current].pred;
    }
    res.push_front(a);
    return res;
}

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

list<int> Graph::minimumLines(int a, int b) {
    list<int> res;
    int distance = dijkstra_lineChange(a, b);
    if (distance == -1)
        return res;
    int current = b;
    while(current != a){
        res.push_front(current);
        current = nodes[current].pred;
    }
    res.push_front(a);
    return res;
}

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
                q.decreaseKey(v.dest, nodes[v.dest].dist);
            }
        }
    }
    if (nodes[b].dist == INF)
        return -1;
    else
        return nodes[b].dist;
}

list<int> Graph::dijkstra_pathMinDistance(int a, int b) {
    list<int> path;
    dijkstra_distanceMinDistance(a, b);
    if (nodes[b].dist == INF)
        return path;
    int current = b;
    while(current != a) {
        path.push_front(current);
        current = nodes[current].pred;
    }
    path.push_front(a);
    return path;
}

list<int> Graph::minimumDistance(int a, int b) {
    return dijkstra_pathMinDistance(a, b);
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
                q.decreaseKey(v.dest, nodes[v.dest].dist);
            }
        }
    }
    if (nodes[b].dist == INF)
        return -1;
    else
        return nodes[b].dist;
}

list<int> Graph::dijkstra_pathMinZones(int a, int b) {
    list<int> path;
    dijkstra_distanceMinZones(a, b);
    if (nodes[b].dist == INF)
        return path;
    int current = b;
    while(current != a) {
        path.push_front(current);
        current = nodes[current].pred;
    }
    path.push_front(a);
    return path;
}

list<int> Graph::minimumZones(int a, int b) {
    return dijkstra_pathMinZones(a, b);
}
