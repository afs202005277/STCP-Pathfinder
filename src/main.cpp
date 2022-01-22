#include <iostream>
#include "FileReader.h"



int main() {
    // Set Up:
    FileReader fileReader("../dataset/stops.csv", "../dataset/lines.csv");
    vector<Stop> stops = fileReader.readStops();
    Graph g(stops.size(), true); // Creates the graph with the required nodes
    unordered_map<string, int> map = fileReader.getMap(); // Map that links a Stop Code with the corresponding node (integer)
    fileReader.readEdges(g); // Adds the edges to the graph

    // Usage:

}
