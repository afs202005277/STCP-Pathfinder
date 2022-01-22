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

void Application::addEdges(const string &path) {
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
        g.addEdge(stopToInt[firstStop], stopToInt[secondStop], path.substr(path.find('_') + 1), path.find('.'),
                  getDistance(stops[stopToInt[firstStop]].getLatitude(), stops[stopToInt[firstStop]].getLongitude(),
                  stops[stopToInt[secondStop]].getLatitude(), stops[stopToInt[secondStop]].getLongitude()));
        firstStop = secondStop;
    }
}

void Application::readEdges() {
    ifstream linesDataset;
    linesDataset.open(linesPath);
    string line;
    getline(linesDataset, line);
    while(getline(linesDataset, line)) {
        addEdges("../dataset/line_" + line.substr(0, line.find(',')) + "_0.csv");
        addEdges("../dataset/line_" + line.substr(0, line.find(',')) + "_1.csv");
    }
}

pair<string, int> Application::getNearestStop(double lat, double lon) {
    Stop tmp;
    double min = DBL_MAX;
    int idx=-1;
    for (int i=1;i<stops.size();i++){
        double dist = getDistance(lat, lon, stops[i].getLatitude(), stops[i].getLongitude());
        if ( dist < min){
            min = dist;
            tmp = stops[i];
            idx = i;
        }
    }
    return {tmp.getCode(), idx};
}

Application::Application(string stopsPath, string linesPath, double distance) : stopsPath(std::move(stopsPath)),
                                                                             linesPath(std::move(linesPath)) {
    walkingDistance = distance;
    readStops();
    g = Graph (stops.size()-1, true);
    readEdges();
    if (walkingDistance > 0)
        addOnFootEdges();
}

list<int> Application::courseWithMinimumStops(string stop1, string stop2) {
    return g.minimumStops(stopToInt[stop1], stopToInt[stop2]);
}

list<pair<string, int>> Application::getAllStopsCloserToXMetres(double lat, double lon, double x) {
    list<pair<string, int>> res;
    for (int i=1;i<stops.size();i++){
        if (getDistance(lat, lon, stops[i].getLatitude(), stops[i].getLongitude()) * 1000 < x) {
            res.emplace_back(stops[i].getCode(), i);
        }
    }
    return res;
}

void Application::addOnFootEdges() {
    for (int i=1;i+1<stops.size();i++){
        for (int j=i+1;j<stops.size();j++){
            double d = getDistance(stops[i].getLatitude(), stops[i].getLongitude(), stops[j].getLatitude(), stops[j].getLongitude());
            if (d * 1000 <= walkingDistance) {
                g.addEdge(i, j, "", d, true);
                g.addEdge(j, i, "", d, true);
            }
        }
    }
}

list<list<int>> Application::courseWithMinimumStops(double lat1, double lon1, double lat2, double lon2) {
    list<list<int>> res;
    list<pair<string, int>> src = getAllStopsCloserToXMetres(lat1, lon1, walkingDistance);
    list<pair<string, int>> dest = getAllStopsCloserToXMetres(lat2, lon2, walkingDistance);
    if (src.empty())
        src.push_back(getNearestStop(lat1, lon1));
    if (dest.empty())
        dest.push_back(getNearestStop(lat2, lon2));
    for (const auto& s:src){
        for (const auto& d:dest){
            res.push_back(courseWithMinimumStops(s.first, d.first));
        }
    }
    return res;
}

int Application::getConnectedComponents() {
    return g.connectedComponents();
}

const vector<Stop> &Application::getStops() const {
    return stops;
}
