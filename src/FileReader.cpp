//
// Created by andre on 19/01/2022.
//

#include <fstream>
#include "FileReader.h"

FileReader::FileReader(string filePath) : filePath(std::move(filePath)) {}

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
    fileStream.open(filePath);
    string line;
    getline(fileStream, line);
    while(getline(fileStream, line)){
        stops.push_back(getStop(line));
    }
    return stops;
}


