#include <iostream>
#include "FileReader.h"



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
