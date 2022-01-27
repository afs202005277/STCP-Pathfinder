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
        int weight = 0;
        if (stops[stopToInt[firstStop]].getZone() != stops[stopToInt[secondStop]].getZone())
            weight = 1;
        g.addEdge(stopToInt[firstStop], stopToInt[secondStop], path.substr(path.find('_') + 1), path.find('.'),
                  getDistance(stops[stopToInt[firstStop]].getLatitude(), stops[stopToInt[firstStop]].getLongitude(),
                  stops[stopToInt[secondStop]].getLatitude(), stops[stopToInt[secondStop]].getLongitude()), weight);
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

list<pair<string, int>> Application::getAllStopsCloserToXMetres(double lat, double lon, double x) {
    list<pair<string, int>> res;
    for (int i=1;i<stops.size();i++){
        if (getDistance(lat, lon, stops[i].getLatitude(), stops[i].getLongitude()) * 1000 < x) {
            res.emplace_back(stops[i].getCode(), i);
        }
    }
    return res;
}

list<int> Application::courseWithMinimumLines(string stop1, string stop2) {
    return g.minimumLines(stopToInt[stop1], stopToInt[stop2]);
}

list<int> Application::courseWithMinimumLines(double lat1, double lon1, double lat2, double lon2)
{
    list<int> res;
    list<pair<string, int>> src = getAllStopsCloserToXMetres(lat1, lon1, walkingDistance);
    list<pair<string, int>> dest = getAllStopsCloserToXMetres(lat2, lon2, walkingDistance);
    if (src.empty())
        src.push_back(getNearestStop(lat1, lon1));
    if (dest.empty())
        dest.push_back(getNearestStop(lat2, lon2));

    int min_lines = -1;
    double min_dist = -1;
    for (const auto& s:src) {
        for (const auto& d:dest) {
            auto tmp = courseWithMinimumLines(s.first, d.first);
            auto linesCross = getLineChange(tmp);
            if (min_lines > linesCross || min_lines == -1)
            {
                res = tmp;
                min_lines = linesCross;
                min_dist = getTotalDistance(tmp) + getDistance(stops[s.second].getLatitude(), stops[s.second].getLongitude(), lat1, lon1) + getDistance(stops[d.second].getLatitude(), stops[d.second].getLongitude(), lat2, lon2);
            }
            else if (min_lines == linesCross)
            {
                double temp = getTotalDistance(tmp) + getDistance(stops[s.second].getLatitude(), stops[s.second].getLongitude(), lat1, lon1) + getDistance(stops[d.second].getLatitude(), stops[d.second].getLongitude(), lat2, lon2);
                if (min_dist > temp || min_dist == -1)
                {
                    res = tmp;
                    min_dist = getTotalDistance(tmp) + getDistance(stops[s.second].getLatitude(), stops[s.second].getLongitude(), lat1, lon1) + getDistance(stops[d.second].getLatitude(), stops[d.second].getLongitude(), lat2, lon2);
                }
            }
        }
    }
    return res;
}

void Application::addOnFootEdges() {
    for (int i=1;i+1<stops.size();i++){
        for (int j=i+1;j<stops.size();j++){
            double d = getDistance(stops[i].getLatitude(), stops[i].getLongitude(), stops[j].getLatitude(), stops[j].getLongitude());
            if (d * 1000 <= walkingDistance) {
                int weight = 0;
                if (stops[i].getZone() != stops[j].getZone())
                    weight = 1;
                g.addEdge(i, j, "", d, true, weight);
                g.addEdge(j, i, "", d, true, weight);
            }
        }
    }
}

int Application::getConnectedComponents() {
    return g.connectedComponents();
}

const vector<Stop> &Application::getStops() const {
    return stops;
}

double Application::getTotalDistance(list<int> l) {
    if (l.empty() || l.size() == 1)
        return 0;
    double total = 0.0;
    int first = l.front();
    for (auto it = next(l.begin());it != l.end();it++) {
        total += getDistance(stops[first].getLatitude(), stops[first].getLongitude(), stops[*it].getLatitude(), stops[*it].getLongitude());
        first = *it;
    }
    return total;
}

int Application::getLineChange(list<int> l)
{
    if (l.empty() || l.size() == 1)
        return 0;
    int lines = 0;
    string line = "";
    auto prev_it = l.begin();
    auto it = l.begin();
    it++;
    for (; it != l.end(); ++it)
    {
        if (g.edgeBetween(*prev_it, *it).line != line)
        {
            line = g.edgeBetween(*prev_it, *it).line;
            lines++;
        }
        prev_it++;
    }
    return lines;
}

list<int> Application::courseWithMinimumDistance(const string& stop1, const string& stop2) {
    auto l = g.minimumDistance(stopToInt[stop1], stopToInt[stop2]);
    return l;
}

list<int> Application::courseWithMinimumDistance(double lat1, double lon1, double lat2, double lon2) {
    list<int> res;
    double min = DBL_MAX;
    list<pair<string, int>> src = getAllStopsCloserToXMetres(lat1, lon1, walkingDistance);
    list<pair<string, int>> dest = getAllStopsCloserToXMetres(lat2, lon2, walkingDistance);
    if (src.empty())
        src.push_back(getNearestStop(lat1, lon1));
    if (dest.empty())
        dest.push_back(getNearestStop(lat2, lon2));
    for (const auto& s:src){
        for (const auto& d:dest){
            auto tmp = courseWithMinimumDistance(s.first, d.first);
            auto distance = getTotalDistance(tmp) + getDistance(stops[s.second].getLatitude(), stops[s.second].getLongitude(), lat1, lon1) + getDistance(stops[d.second].getLatitude(), stops[d.second].getLongitude(), lat2, lon2);
            if (distance < min) {
                min = distance;
                res = tmp;
            }
        }
    }
    return res;
}
