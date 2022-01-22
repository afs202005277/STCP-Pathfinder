//
// Created by andre on 19/01/2022.
//

#include <fstream>
#include "FileReader.h"

FileReader::FileReader(string stopsPath, string linesPath) : stopsPath(stopsPath), linesPath(linesPath) {}

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

vector<Stop> FileReader::readStops() {
    ifstream fileStream;
    fileStream.open(stopsPath);
    string line;
    getline(fileStream, line);
    int counter = 1;
    while(getline(fileStream, line)){
        Stop tmp = getStop(line);
        stops.push_back(tmp);
        map.insert({tmp.getCode(), counter});
        counter++;
    }
    return stops;
}

const unordered_map<string, int> &FileReader::getMap() const {
    return map;
}

void FileReader::addEdges(Graph &g, const string& path) {
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
        g.addEdge(map[firstStop], map[secondStop]);
        firstStop = secondStop;
    }
}

void FileReader::readEdges(Graph &g) {
    ifstream linesDataset;
    linesDataset.open(linesPath);
    string line;
    getline(linesDataset, line);
    while(getline(linesDataset, line)) {
        addEdges(g, "../dataset/line_" + line.substr(0, line.find(',')) + "_0.csv");
        addEdges(g, "../dataset/line_" + line.substr(0, line.find(',')) + "_1.csv");
    }
}


