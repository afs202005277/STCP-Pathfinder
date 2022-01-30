#include "Application.h"
#include <fstream>
#include <cmath>
#include <cfloat>


Application::Application(string stopsPath, string linesPath, double distance, list<string> forbiddenStops, list<string> forbiddenLines, char nightOrDay) : stopsPath(std::move(stopsPath)),
                                                                                linesPath(std::move(linesPath)) {
    walkingDistance = distance;
    this->forbiddenStops = std::move(forbiddenStops);
    readStops();
    g = Graph (stops.size()-1, std::move(forbiddenLines),true);
    if (nightOrDay == 'D')
        day = true;
    else
        day = false;
    readEdges();
    if (walkingDistance > 0)
        addOnFootEdges();
}

Stop Application::getStop(string line){
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

bool Application::canUse(string code) {
    for (string c:forbiddenStops) {
        if (c == code)
            return false;
    }
    return true;
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
        if (canUse(tmp.getCode())) {
            stops.push_back(tmp);
            stopToInt.insert({tmp.getCode(), counter});
            counter++;
        }
    }
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

void Application::readEdges() {
    ifstream linesDataset;
    linesDataset.open(linesPath);
    string line;
    getline(linesDataset, line);
    while(getline(linesDataset, line)) {
        string lineCode = line.substr(0, line.find(','));
        if (day && lineCode.find('M') == string::npos) {
            addEdges("../dataset/line_" + lineCode + "_0.csv");
            addEdges("../dataset/line_" + lineCode + "_1.csv");
        }
        else if (!day && lineCode.find('M') != string::npos) {
            addEdges("../dataset/line_" + lineCode + "_0.csv");
            addEdges("../dataset/line_" + lineCode + "_1.csv");
        }
    }
}

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

pair<int, list<Edge>> Application::courseWithMinimumStops(const string& stop1, const string& stop2) {
    if (stopToInt.find(stop1) == stopToInt.end() || stopToInt.find(stop2) == stopToInt.end()) {
        cout << "Invalid code provided!" << endl;
        return {};
    }
    return {stopToInt[stop1],g.minimumStops(stopToInt[stop1], stopToInt[stop2])};
}

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

list<pair<string, int>> Application::getAllStopsCloserToXMetres(double lat, double lon, double x) {
    list<pair<string, int>> res;
    for (int i=1;i<stops.size();i++){
        if (getDistance(lat, lon, stops[i].getLatitude(), stops[i].getLongitude()) * 1000 < x) {
            res.emplace_back(stops[i].getCode(), i);
        }
    }
    return res;
}

pair<int, list<Edge>> Application::courseWithMinimumLines(const string& stop1, const string& stop2) {
    if (stopToInt.find(stop1) == stopToInt.end() || stopToInt.find(stop2) == stopToInt.end()) {
        cout << "Invalid code provided!" << endl;
        return {};
    }
    return {stopToInt[stop1],g.minimumLines(stopToInt[stop1], stopToInt[stop2])};
}

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
    for (const auto& s:src) {
        for (const auto& d:dest) {
            auto tmp = courseWithMinimumLines(s.first, d.first);
            auto linesCross = getLineChange(tmp.second);
            if (min_lines > linesCross || min_lines == -1)
            {
                res = tmp.second;
                min_lines = linesCross;
                source = s.second;
            }
        }
    }
    return {source, res};
}

int Application::getConnectedComponents() {
    return g.connectedComponents();
}

const vector<Stop> &Application::getStops() const {
    return stops;
}

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

pair<int, list<Edge>> Application::courseWithMinimumDistance(const string& stop1, const string& stop2) {
    if (stopToInt.find(stop1) == stopToInt.end() || stopToInt.find(stop2) == stopToInt.end()) {
        cout << "Invalid code provided!" << endl;
        return {};
    }
    return {stopToInt[stop1] ,g.dijkstra_pathMinDistance(stopToInt[stop1], stopToInt[stop2])};
}

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

pair<int, list<Edge>> Application::courseWithMinimumZones(const string& stop1, const string& stop2) {
    if (stopToInt.find(stop1) == stopToInt.end() || stopToInt.find(stop2) == stopToInt.end()) {
        cout << "Invalid code provided!" << endl;
        return {};
    }
    return {stopToInt[stop1],g.dijkstra_pathMinZones(stopToInt[stop1], stopToInt[stop2])};
}

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

pair<double, vector<int>> Application::MST(const string& stop) {
    if (stopToInt.find(stop) == stopToInt.end())
        return {};
    return g.prim(stopToInt[stop]);
}