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
    list<string> forbiddenStops;
    bool day;

public:
    Application(string stopsPath, string linesPath, double distance, list<string> forbiddenStops, list<string> forbiddenLines, char nightOrDay);
    void readStops();
    void readEdges();
    void addEdges(const string &path);
    list<pair<string, int>> getAllStopsCloserToXMetres(double lat, double lon, double x); //par de code da paragem e respetivo indice no vetor
    void addOnFootEdges();
    int getConnectedComponents();
    pair<string, int> getNearestStop(double lat, double lon);
    const vector<Stop> &getStops() const;
    double getTotalDistance(int src, const list<Edge>& l);
    int getLineChange(list<Edge> l);
    int getTotalZoneChanges(int src, list<Edge> l);
    double getDistance(double lat1, double lon1, double lat2, double lon2);

    pair<int, list<Edge>> courseWithMinimumDistance(double lat1, double lon1, double lat2, double lon2);
    pair<int, list<Edge>> courseWithMinimumDistance(const string& stop1, double lat2, double lon2);
    pair<int, list<Edge>> courseWithMinimumDistance(double lat1, double lon1, const string& stop2);
    pair<int, list<Edge>> courseWithMinimumDistance(const string& stop1, const string& stop2);

    pair<int, list<Edge>> courseWithMinimumStops(double lat1, double lon1, double lat2, double lon2);
    pair<int, list<Edge>> courseWithMinimumStops(const string& stop1, double lat2, double lon2);
    pair<int, list<Edge>> courseWithMinimumStops(double lat1, double lon1, const string& stop2);
    pair<int, list<Edge>> courseWithMinimumStops(const string& stop1, const string& stop2);

    pair<int, list<Edge>> courseWithMinimumLines(double lat1, double lon1, double lat2, double lon2);
    pair<int, list<Edge>> courseWithMinimumLines(const string& stop1, double lat2, double lon2);
    pair<int, list<Edge>> courseWithMinimumLines(double lat1, double lon1, string stop2);
    pair<int, list<Edge>> courseWithMinimumLines(const string& stop1, const string& stop2);

    pair<int, list<Edge>> courseWithMinimumZones(double lat1, double lon1, double lat2, double lon2);
    pair<int, list<Edge>> courseWithMinimumZones(const string& stop1, double lat2, double lon2);
    pair<int, list<Edge>> courseWithMinimumZones(double lat1, double lon1, const string& stop2);
    pair<int, list<Edge>> courseWithMinimumZones(const string& stop1, const string& stop2);

    pair<double, list<int>> MST(const string& stop);

    bool canUse(string code);
};


#endif //PROJETO2_APPLICATION_H
