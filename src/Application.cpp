//
// Created by andre on 22/01/2022.
//

#include "Application.h"
#include <fstream>
#include <cmath>
#include <cfloat>


Application::Application(string stopsPath, string linesPath, double distance) : stopsPath(std::move(stopsPath)),
                                                                                linesPath(std::move(linesPath)) {
    walkingDistance = distance;
    readStops();
    g = Graph (stops.size()-1, true);
    readEdges();
    if (walkingDistance > 0)
        addOnFootEdges();
}

/**
 * Creates a Stop object using the information received
 * @param line string that contains all the information of a certain bus stop
 * @return the created object
 */
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

/**
 * Function that fills the stops vector with all the stops present in the file
 */
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

/**
 * Function that adds all the links between stops that are materialized by foot
 */
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

/**
 * Function that reads the existing lines that connect the different bus stops
 * @param path string that contains the path to the file with the corresponding route of that line
 */
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
        g.addEdge(stopToInt[firstStop], stopToInt[secondStop], path.substr(path.find('_') + 1, path.find('.')-path.find('_')),
                  getDistance(stops[stopToInt[firstStop]].getLatitude(), stops[stopToInt[firstStop]].getLongitude(),
                              stops[stopToInt[secondStop]].getLatitude(), stops[stopToInt[secondStop]].getLongitude()), false, weight);
        firstStop = secondStop;
    }
}

/**
 * Function that reads the lines from the lines.csv file
 */
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

/**
 * Function that calculates the distance between two points using its coordinates
 * @param lat1 the latitude of point 1
 * @param lon1 the longitude of point 1
 * @param lat2 the latitude of point 2
 * @param lon2 the longitude of point 2
 * @return the distance between two points in kilometers (double)
 */
double Application::getDistance(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = (lat1) * M_PI / 180.0;
    lat2 = (lat2) * M_PI / 180.0;
    double a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    double rad = 6371;
    double c = 2 * asin(sqrt(a));
    return rad * c;
}

/**
 * Function that returns the nearest stop to the coordinates given
 * @param lat latitude of the point
 * @param lon longitude of the point
 * @return pair where where the first element is the stop's code and the second one is the index of that stop in the stops vector
 */
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

/**
 * Function that calculates the course (stop1 -> targetStop2) that minimizes the amount of stops used
 * @param stop1 source stop
 * @param stop2 destination stop
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumStops(const string& stop1, const string& stop2) {
    return {stopToInt[stop1],g.minimumStops(stopToInt[stop1], stopToInt[stop2])};
}

/**
 * Function that calculates the course (startingPoint -> targetStop) that minimizes the amount of stops used
 * @param lat1 the latitude of the starting point
 * @param lon1 the longitude of the starting point
 * @param stop2 destination stop
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumStops(double lat1, double lon1, double lat2, double lon2) {
    list<Edge> res, tmp;
    int min = INT_MAX, source;
    list<pair<string, int>> src = getAllStopsCloserToXMetres(lat1, lon1, walkingDistance);
    list<pair<string, int>> dest = getAllStopsCloserToXMetres(lat2, lon2, walkingDistance);
    if (src.empty())
        src.push_back(getNearestStop(lat1, lon1));
    if (dest.empty())
        dest.push_back(getNearestStop(lat2, lon2));
    for (const auto& s:src){
        for (const auto& d:dest){
            tmp = courseWithMinimumStops(s.first, d.first).second;
            if (tmp.size() < min){
                res = tmp;
                min = tmp.size();
                source = s.second;
            }
        }
    }
    return {source, res};
}

/**
 * Function that returns all the stops that are closer to X metres to a given point
 * @param lat the latitude of the starting point
 * @param lon the longitude of the starting point
 * @param x the maximum distance
 * @return a list with pairs where the first element is the stop code and the second is the corresponding index in the stops vector
 */
list<pair<string, int>> Application::getAllStopsCloserToXMetres(double lat, double lon, double x) {
    list<pair<string, int>> res;
    for (int i=1;i<stops.size();i++){
        if (getDistance(lat, lon, stops[i].getLatitude(), stops[i].getLongitude()) * 1000 < x) {
            res.emplace_back(stops[i].getCode(), i);
        }
    }
    return res;
}

/**
 * Function that calculates the course (stop1 -> stop2) that minimizes the times the user changes lines
 * @param stop1 the starting bus stop
 * @param stop2 the destination bus stop
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumLines(const string& stop1, const string& stop2) {
    return {stopToInt[stop1],g.minimumLines(stopToInt[stop1], stopToInt[stop2])};
}

/**
 * Function that calculates the course (startingPoint -> targetPoint) that minimizes the times the user changes lines
 * @param lat1 latitude of the startingPoint
 * @param lon1 longitude of the startingPoint
 * @param lat2 latitude of the targetPoint
 * @param lon2 longitude of the targetPoint
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumLines(double lat1, double lon1, double lat2, double lon2)
{
    list<Edge> res;
    int source;
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
            auto linesCross = getLineChange(tmp.second);
            if (min_lines > linesCross || min_lines == -1)
            {
                res = tmp.second;
                min_lines = linesCross;
                min_dist = getTotalDistance(s.second, tmp.second) + getDistance(stops[s.second].getLatitude(), stops[s.second].getLongitude(), lat1, lon1) + getDistance(stops[d.second].getLatitude(), stops[d.second].getLongitude(), lat2, lon2);
                source = s.second;
            }
            else if (min_lines == linesCross)
            {
                double temp = getTotalDistance(s.second, tmp.second) + getDistance(stops[s.second].getLatitude(), stops[s.second].getLongitude(), lat1, lon1) + getDistance(stops[d.second].getLatitude(), stops[d.second].getLongitude(), lat2, lon2);
                if (min_dist > temp || min_dist == -1)
                {
                    res = tmp.second;
                    min_dist = getTotalDistance(s.second, tmp.second) + getDistance(stops[s.second].getLatitude(), stops[s.second].getLongitude(), lat1, lon1) + getDistance(stops[d.second].getLatitude(), stops[d.second].getLongitude(), lat2, lon2);
                    source = s.second;
                }
            }
        }
    }
    return {source, res};
}

/**
 * Function that returns the amount of connected components of the graph
 * @return amount (integer) of connected components of the graph
 */
int Application::getConnectedComponents() {
    return g.connectedComponents();
}

const vector<Stop> &Application::getStops() const {
    return stops;
}

/**
 * Function that calculates the total distance travelled in the course received as parameter
 * @param l list of integers that represent the course to be analyzed
 * @return total distance travelled in the course
 */
double Application::getTotalDistance(int src, const list<Edge>& l) {
    if (l.empty() || l.size() == 1)
        return 0;
    double total = 0.0;
    int first = src;
    for (auto & elem : l) {
        total += getDistance(stops[first].getLatitude(), stops[first].getLongitude(), stops[elem.dest].getLatitude(), stops[elem.dest].getLongitude());
        first = elem.dest;
    }
    return total;
}

/**
 * Function that calculates the total number of times the user has changed lines
 * @param l list of integers that represent the course to be analyzed
 * @return total number of times the user has changed lines
 */
int Application::getLineChange(list<Edge> l)
{
    if (l.empty() || l.size() == 1)
        return 0;
    int lines = 0;
    string prevLine = l.front().line;
    for (auto it = next(l.begin());it!=l.end();it++){
        if (it->line != prevLine){
            lines++;
            prevLine = it->line;
        }
    }
    return lines;
}

/**
 * Function that calculates the course (stop1 -> stop2) that minimizes the distance travelled
 * @param stop1 the starting bus stop
 * @param stop2 the destination bus stop
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumDistance(const string& stop1, const string& stop2) {
    return {stopToInt[stop1] ,g.dijkstra_pathMinDistance(stopToInt[stop1], stopToInt[stop2])};
}

/**
 * Function that calculates the course (startingPoint -> targetPoint) that minimizes the times the user changes lines
 * @param lat1 the latitude of the starting point
 * @param lon1 the longitude of the starting point
 * @param lat2 the latitude of the target point
 * @param lon2 the longitude of the target point
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumDistance(double lat1, double lon1, double lat2, double lon2) {
    list<Edge> res;
    int source;
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
            auto distance = getTotalDistance(s.second, tmp.second) + getDistance(stops[s.second].getLatitude(), stops[s.second].getLongitude(), lat1, lon1) + getDistance(stops[d.second].getLatitude(), stops[d.second].getLongitude(), lat2, lon2);
            if (distance < min) {
                min = distance;
                res = tmp.second;
                source = s.second;
            }
        }
    }
    return {source, res};
}

/**
 * Function that calculates the course (stop1 -> stop2) that minimizes the zones used
 * @param stop1 the starting bus stop
 * @param stop2 the destination bus stop
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumZones(const string& stop1, const string& stop2) {
    return {stopToInt[stop1],g.dijkstra_pathMinZones(stopToInt[stop1], stopToInt[stop2])};
}

/**
 * Function that calculates the course (stop1 -> stop2) that minimizes the zones used
 * @param lat1 the latitude of the starting point
 * @param lon1 the longitude of the starting point
 * @param lat2 the latitude of the target point
 * @param lon2 the longitude of the target point
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumZones(double lat1, double lon1, double lat2, double lon2) {
    list<Edge> res;
    int min = INT_MAX, source;
    list<pair<string, int>> src = getAllStopsCloserToXMetres(lat1, lon1, walkingDistance);
    list<pair<string, int>> dest = getAllStopsCloserToXMetres(lat2, lon2, walkingDistance);
    if (src.empty())
        src.push_back(getNearestStop(lat1, lon1));
    if (dest.empty())
        dest.push_back(getNearestStop(lat2, lon2));
    for (const auto& s:src){
        for (const auto& d:dest){
            auto tmp = courseWithMinimumZones(s.first, d.first);
            auto zones = getTotalZoneChanges(s.second, tmp.second);
            if (zones < min) {
                min = zones;
                res = tmp.second;
                source = s.second;
            }
        }
    }
    return {source, res};
}

/**
 * Function that calculates the total number of times the user has changed zones
 * @param l list of integers that represent the course to be analyzed
 * @return the total number of times the user has changed zones
 */
int Application::getTotalZoneChanges(int src, list<Edge> l) {
    if (l.empty() || l.size() == 1)
        return 0;
    int total = 0;
    int first = src;
    for (auto & it : l) {
        total += stops[first].getZone() != stops[it.dest].getZone();
        first = it.dest;
    }
    return total;
}

pair<int, list<Edge>> Application::courseWithMinimumLines(double lat1, double lon1, string stop2) {
    auto src = getAllStopsCloserToXMetres(lat1, lon1, walkingDistance);
    list<Edge> res;
    int min = INT_MAX, source;
    if (src.empty())
        src.push_back(getNearestStop(lat1, lon1));
    for (auto l:src) {
        auto tmp = courseWithMinimumLines(l.first, stop2);
        int d = getLineChange(tmp.second);
        if (d < min){
            res = tmp.second;
            source = tmp.first;
            min = d;
        }
    }
    return {source, res};
}

pair<int, list<Edge>> Application::courseWithMinimumLines(const string& stop1, double lat2, double lon2) {
    auto dest = getAllStopsCloserToXMetres(lat2, lon2, walkingDistance);
    if (dest.empty())
        dest.push_back(getNearestStop(lat2, lon2));
    list<Edge> res;
    int min = INT_MAX, source;
    for (auto l:dest) {
        auto tmp = courseWithMinimumLines(stop1, l.first);
        int d = getLineChange(tmp.second);
        if (d < min) {
            res = tmp.second;
            min = d;
            source = tmp.first;
        }
    }
    return {source, res};
}

/**
 * Function that calculates the course (stop1 -> targetPoint) that minimizes the distance travelled
 * @param stop1 the starting bus stop
 * @param lat2 the latitude of the target point
 * @param lon2 the longitude of the target point
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumDistance(const string& stop1, double lat2, double lon2) {
    auto dest = getAllStopsCloserToXMetres(lat2, lon2, walkingDistance);
    if (dest.empty())
        dest.push_back(getNearestStop(lat2, lon2));
    list<Edge> temp, res;
    int min = INT_MAX;
    for (auto l:dest) {
        temp = g.dijkstra_pathMinDistance(stopToInt[stop1], stopToInt[l.first]);
        int d = getTotalDistance(stopToInt[stop1], temp);
        if (d < min){
            res = temp;
            min = d;
        }
    }
    return {stopToInt[stop1],res};
}

/**
 * Function that calculates the course (startingPoint -> stop2) that minimizes the distance travelled
 * @param lat1 the latitude of the starting point
 * @param lon1 the longitude of the starting point
 * @param stop2 the destination bus stop
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumDistance(double lat1, double lon1, const string &stop2) {
    auto src = getAllStopsCloserToXMetres(lat1, lon1, walkingDistance);
    if (src.empty())
        src.push_back(getNearestStop(lat1, lon1));
    list<Edge> temp, res;
    int source;
    int min = INT_MAX;
    for (auto l:src) {
        temp = g.dijkstra_pathMinDistance(stopToInt[l.first], stopToInt[stop2]);
        int d = getTotalDistance(l.second, temp);
        if (d < min){
            res = temp;
            min = d;
            source = l.second;
        }
    }
    return {source, res};
}

/**
 * Function that calculates the course (stop1 -> targetPoint) that minimizes the amount of stops used
 * @param stop1 the starting bus stop
 * @param lat2 the latitude of the target point
 * @param lon2 the longitude of the target point
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumStops(const string& stop1, double lat2, double lon2) {
    auto dest = getAllStopsCloserToXMetres(lat2, lon2, walkingDistance);
    if (dest.empty())
        dest.push_back(getNearestStop(lat2, lon2));
    list<Edge> temp, res;
    int min = INT_MAX;
    for (auto l:dest) {
        temp = g.minimumStops(stopToInt[stop1], stopToInt[l.first]);
        int d = temp.size();
        if (d < min){
            res = temp;
            min = d;
        }
    }
    return {stopToInt[stop1],res};
}

/**
 * Function that calculates the course (startingPoint -> stop2) that minimizes the amount of stops used
 * @param lat1 the latitude of the starting point
 * @param lon1 the longitude of the starting point
 * @param stop2 the destination bus stop
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumStops(double lat1, double lon1, const string& stop2) {
    auto src = getAllStopsCloserToXMetres(lat1, lon1, walkingDistance);
    if (src.empty())
        src.push_back(getNearestStop(lat1, lon1));
    list<Edge> temp, res;
    int min = INT_MAX, source;
    for (auto l:src) {
        temp = g.minimumStops(stopToInt[l.first], stopToInt[stop2]);
        int d = temp.size();
        if (d < min){
            res = temp;
            min = d;
            source = l.second;
        }
    }
    return {source, res};
}

/**
 * Function that calculates the course (startingPoint -> stop2) that minimizes the amount of zones used
 * @param stop1 the starting bus stop
 * @param lat2 the latitude of the target point
 * @param lon2 the longitude of the target point
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumZones(const string &stop1, double lat2, double lon2) {
    auto dest = getAllStopsCloserToXMetres(lat2, lon2, walkingDistance);
    if (dest.empty())
        dest.push_back(getNearestStop(lat2, lon2));
    list<Edge> temp, res;
    int min = INT_MAX;
    for (auto l:dest) {
        temp = g.dijkstra_pathMinZones(stopToInt[stop1], stopToInt[l.first]);
        int d = getTotalZoneChanges(stopToInt[stop1], temp);
        if (d < min){
            res = temp;
            min = d;
        }
    }
    return {stopToInt[stop1],res};
}

/**
 * Function that calculates the course (startingPoint -> stop2) that minimizes the amount of zones used
 * @param lat1 the latitude of the starting point
 * @param lon1 the longitude of the starting point
 * @param stop2 the destination bus stop
 * @return a list with integers corresponding to the different bus stops used
 */
pair<int, list<Edge>> Application::courseWithMinimumZones(double lat1, double lon1, const string &stop2) {
    auto src = getAllStopsCloserToXMetres(lat1, lon1, walkingDistance);
    if (src.empty())
        src.push_back(getNearestStop(lat1, lon1));
    list<Edge> temp, res;
    int min = INT_MAX, source;
    for (auto l:src) {
        temp = g.dijkstra_pathMinZones(stopToInt[l.first], stopToInt[stop2]);
        int d = getTotalZoneChanges(l.second,temp);
        if (d < min){
            res = temp;
            min = d;
            source = l.second;
        }
    }
    return {source, res};
}

pair<double, list<int>> Application::MST(const string& stop) {
    return g.prim(stopToInt[stop]);
}