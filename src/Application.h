//
// Created by andre on 22/01/2022.
//

#ifndef PROJETO2_APPLICATION_H
#define PROJETO2_APPLICATION_H

#include <vector>
#include <unordered_map>
#include "graph.h"
#include "Stop.h"

using namespace std;

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
    void readEdges();
    void addEdges(const string &path);
    list<pair<string, int>> getAllStopsCloserToXMetres(double lat, double lon, double x); //par de code da paragem e respetivo indice no vetor
    void addOnFootEdges();
    int getConnectedComponents();
    pair<string, int> getNearestStop(double lat, double lon);
    const vector<Stop> &getStops() const;
    double getTotalDistance(list<int> l);
    int getLineChange(list<int> l);

    list<int> courseWithMinimumDistance(double lat1, double lon1, double lat2, double lon2);
    list<int> courseWithMinimumDistance(const string& stop1, double lat2, double lon2);
    list<int> courseWithMinimumDistance(double lat1, double lon1, const string& stop2);
    list<int> courseWithMinimumDistance(const string& stop1, const string& stop2);

    list<int> courseWithMinimumStops(double lat1, double lon1, double lat2, double lon2);
    list<int> courseWithMinimumStops(string stop1, double lat2, double lon2);
    list<int> courseWithMinimumStops(double lat1, double lon1, string stop2);
    list<int> courseWithMinimumStops(const string& stop1, const string& stop2);

    list<int> courseWithMinimumLines(double lat1, double lon1, double lat2, double lon2);
    list<int> courseWithMinimumLines(string stop1, double lat2, double lon2);
    list<int> courseWithMinimumLines(double lat1, double lon1, string stop2);
    list<int> courseWithMinimumLines(const string& stop1, const string& stop2);

    list<int> courseWithMinimumZones(double lat1, double lon1, double lat2, double lon2);
    list<int> courseWithMinimumZones(const string& stop1, double lat2, double lon2);
    list<int> courseWithMinimumZones(double lat1, double lon1, const string& stop2);
    list<int> courseWithMinimumZones(const string& stop1, const string& stop2);

    int getTotalChanges(list<int> l);

    double getDistance(double lat1, double lon1, double lat2, double lon2);
};


#endif //PROJETO2_APPLICATION_H
