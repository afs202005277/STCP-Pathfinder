#include "graph.h"
#include "minHeap.h"
#include <cfloat>
#define INF (INT_MAX/2)


Graph::Graph(int num, list<string> forbiddenLines, bool dir) : n(num), hasDir(dir), nodes(num+1), forbiddenLines(std::move(forbiddenLines)) {}

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

bool Graph::canUse(const string& line) {
    for (string l:forbiddenLines)
        if (l==line)
            return false;
    return true;
}

void Graph::addEdge(int src, int dest, string line, double d, bool foot, int weight) {
    line = line.substr(0, line.find('.'));
    if (!canUse(line))
        return;
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

pair<double, vector<int>> Graph::prim(int r) {
    for (int i=1;i<=n;i++) {
        nodes[i].dist = DBL_MAX;
        nodes[i].pred = 0;
    }
    nodes[r].dist = 0.0;
    MinHeap<int, double> Q(n, -1.0);
    for (int i=1;i<=n;i++)
        Q.insert(i, nodes[i].dist);
    while(Q.getSize()>0){
        int u = Q.removeMin();
        for (const auto& v:nodes[u].adj){
            if (Q.hasKey(v.dest) and v.distanceRealWorld < nodes[v.dest].dist)
            {
                nodes[v.dest].pred = u;
                nodes[v.dest].dist = v.distanceRealWorld;
                Q.decreaseKey(v.dest, v.distanceRealWorld);
            }
        }
    }
    double distance = 0.0;
    vector<int> res;
    bool flag = true;
    for (int i=1;i<=n;i++){
        if (nodes[i].dist == DBL_MAX) {
            if (flag) {
                cout << "With the settings provided, Prim's algorithm wasn't able to reach all the nodes (this happened because the graph has "<< connectedComponents() << " different connected components), therefore these nodes will be ignored" << endl;
                flag = false;
            }
        }
        else {
            res.push_back(i);
            distance += nodes[i].dist;
        }
    }
    return {distance, res};
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
                nodes[w].edgePrev = e;
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

list<Edge> Graph::minimumLines(int a, int b) {
    list<Edge> res;
    int min_lineChange = 0;
    for (Edge v : nodes[a].adj) {
        list<Edge> temp;
        nodes[a].edgePrev.line = v.line;
        int distance = dijkstra_lineChange(a, b);
        if (distance == -1)
            continue;
        int current = b;
        int lineChange = 0;
        string prevLine = "";
        while (current != a) {
            temp.push_front(nodes[current].edgePrev);
            if (nodes[current].edgePrev.line != prevLine)
            {
                lineChange++;
                prevLine = nodes[current].edgePrev.line;
            }
            current = nodes[current].pred;
        }
        if (res.empty() || lineChange < min_lineChange)
        {
            res = temp;
            min_lineChange = lineChange;
        }
    }
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
            nodes[i].edgePrev = Edge();
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

