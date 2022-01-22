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
    double walkingDistance;

public:
    Application(string stopsPath, string linesPath, double distance);
    void readStops();
    const unordered_map<string, int> &getStopToInt() const;
    void readEdges();
    void addEdges(const string &path);
    list<int> courseWithMinimumStops(string stop1, string stop2);
    list<list<int>> courseWithMinimumStops(double lat1, double lon1, double lat2, double lon2);
    list<pair<string, int>> getAllStopsCloserToXMetres(double lat, double lon, double x); //par de code da paragem e respetivo indice no vetor
    void addOnFootEdges();
    int getConnectedComponents();
    pair<string, int> getNearestStop(double lat, double lon);
};


#endif //PROJETO2_APPLICATION_H
