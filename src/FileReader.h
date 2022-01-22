//
// Created by andre on 19/01/2022.
//

#ifndef PROJETO2_FILEREADER_H
#define PROJETO2_FILEREADER_H
using namespace std;
#include <vector>
#include <unordered_map>

#include "graph.h"
#include "Stop.h"

class FileReader {
private:
    vector<Stop> stops;
    string stopsPath, linesPath;
    unordered_map<string, int> stopToInt;
    unordered_map<int, string> intToStop;

public:
    FileReader(string stopsPath, string linesPath);
    vector<Stop> readStops();
    const unordered_map<string, int> &getStopToInt() const;

    const unordered_map<int, string> &getIntToStop() const;

    void readEdges(Graph &g);
    void addEdges(Graph &g, const string& path);
};


#endif //PROJETO2_FILEREADER_H
