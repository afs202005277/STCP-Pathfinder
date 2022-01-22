//
// Created by andre on 22/01/2022.
//

#ifndef PROJETO2_APPLICATION_H
#define PROJETO2_APPLICATION_H

using namespace std;

#include <vector>
#include <unordered_map>
#include "graph.h"
#include "Stop.h"

class Application {
private:
    vector<Stop> stops;
    string stopsPath, linesPath;
    unordered_map<string, int> stopToInt;
    Graph g;
    int walkingDistance;

public:
    Application(string stopsPath, string linesPath, double distance);
    void readStops();
    const unordered_map<string, int> &getStopToInt() const;
    void readEdges();
    void addEdges(const string &path);
    list<int> courseWithMinimumStops(string stop1, string stop2);
    list<pair<string, int>> getAllStopsCloserToXMetres(double lat, double lon, unsigned int x); //par de code da paragem e respetivo indice no vetor
    void addOnFootEdges();
};


#endif //PROJETO2_APPLICATION_H
