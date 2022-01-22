//
// Created by andre on 22/01/2022.
//

#include "Application.h"
#include <fstream>
#include <cmath>
#include <cfloat>

Stop getStop(string line){
    string code = line.substr(0, line.find(','));
    line = line.substr(line.find(',')+1);
    string name = line.substr(0, line.find(','));
    line = line.substr(line.find(',')+1);
    string zone = line.substr(0, line.find(','));
    line = line.substr(line.find(',')+1);
    double latitude = stod(line.substr(0, line.find(',')));
    line = line.substr(line.find(',')+1);
    double longitude = stod(line);
    Stop stop(code, name, zone, latitude, longitude);
    return stop;
}

void Application::readStops() {
    ifstream fileStream;
    fileStream.open(stopsPath);
    string line;
    getline(fileStream, line);
    stops.emplace_back();
    int counter = 1;
    while(getline(fileStream, line)){
        Stop tmp = getStop(line);
        stops.push_back(tmp);
        stopToInt.insert({tmp.getCode(), counter});
        counter++;
    }
}

const unordered_map<string, int> &Application::getStopToInt() const {
    return stopToInt;
}

void Application::addEdges(Graph &g, const string& path) {
    ifstream trajectory;
    trajectory.open(path);
    if (!trajectory.is_open()) {
        cout << "Failed to open file: " + path << endl;
        return;
    }
    string amountStops, firstStop, secondStop;
    getline(trajectory, amountStops);
    getline(trajectory, firstStop);
    for (int i=0;i<stoi(amountStops);i++) {
        getline(trajectory, secondStop);
        g.addEdge(stopToInt[firstStop], stopToInt[secondStop], path.substr(path.find('_') + 1), path.find('.'));
        firstStop = secondStop;
    }
}

void Application::readEdges(Graph &g) {
    ifstream linesDataset;
    linesDataset.open(linesPath);
    string line;
    getline(linesDataset, line);
    while(getline(linesDataset, line)) {
        addEdges(g, "../dataset/line_" + line.substr(0, line.find(',')) + "_0.csv");
        addEdges(g, "../dataset/line_" + line.substr(0, line.find(',')) + "_1.csv");
    }
}

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
    for (int i=1;i<stops.size();i++){
        double dist = getDistance(lat, lon, stops[i].getLatitude(), stops[i].getLongitude());
        if ( dist < min){
            min = dist;
            tmp = stops[i];
        }
    }
    return tmp.getCode();
}

Application::Application(string stopsPath, string linesPath) : stopsPath(std::move(stopsPath)),
                                                                             linesPath(std::move(linesPath)) {
    readStops();
    g = Graph (stops.size(), true);
    readEdges(g);
}

list<int> Application::courseWithMinimumStops(string stop1, string stop2) {
    return g.minimumStops(stopToInt[stop1], stopToInt[stop2]);
}

list<pair<string, int>> Application::getAllStopsCloserToXMetres(double lat, double lon, unsigned int x) {
    list<pair<string, int>> res;
    for (int i=1;i<stops.size();i++){
        if (static_cast<bool>(getDistance(lat, lon, stops[i].getLatitude(), stops[i].getLongitude() < (double) x))) {
            res.emplace_back(stops[i].getCode(), i);
        }
    }
    return res;
}
