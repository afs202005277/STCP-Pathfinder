#include <iostream>
#include <cmath>
#include <cfloat>
#include "FileReader.h"

double getDistance(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = (lat1) * M_PI / 180.0;
    lat2 = (lat2) * M_PI / 180.0;
    double a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    double rad = 6371;
    double c = 2 * asin(sqrt(a));

    return rad * c;
}

string getNearestStop(double lat, double lon, const vector<Stop> &stops) {
    Stop tmp;
    double min = DBL_MAX;
    for (const auto& s:stops){
        double dist = getDistance(lat, lon, s.getLatitude(), s.getLongitude());
        if ( dist < min){
            min = dist;
            tmp = s;
        }
    }
    return tmp.getCode();
}


int main() {
    // Set Up:
    FileReader fileReader("../dataset/stops.csv", "../dataset/lines.csv");
    vector<Stop> stops = fileReader.readStops();
    Graph g(stops.size(), true); // Creates the graph with the required nodes
    unordered_map<int, string> intToStop = fileReader.getIntToStop(); // Map that links an integer with the corresponding Stop Code
    unordered_map<string, int> stopToInt = fileReader.getStopToInt(); // Map that links a Stop Code with the corresponding node (integer)
    fileReader.readEdges(g); // Adds the edges to the graph

    // Usage:
    list<int> path = g.minimumStops(stopToInt["INF1"], stopToInt["PASS"]);
    if (path.empty())
        cout << "No path" << endl;
    else {
        for (auto i:path)
            cout << intToStop[i] << " ";
        cout << endl;
    }
}
