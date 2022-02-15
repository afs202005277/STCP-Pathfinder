#ifndef PROJETO2_APPLICATION_H
#define PROJETO2_APPLICATION_H

#include <vector>
#include <unordered_map>
#include "graph.h"
#include "Stop.h"

using namespace std;

class Application {
private:
    vector<Stop> stops;
    string stopsPath, linesPath;
    unordered_map<string, int> stopToInt;
    Graph g;
    double walkingDistance;
    list<string> forbiddenStops;
    bool day;

    /**
    * Creates a Stop object using the information received
    * @param line string that contains all the information of a certain bus stop
    * @return the created object
    */
    Stop getStop(string line);

    /**
    * Function that calculates the distance between two points using its coordinates
    * @param lat1 the latitude of point 1
    * @param lon1 the longitude of point 1
    * @param lat2 the latitude of point 2
    * @param lon2 the longitude of point 2
    * @return the distance between two points in kilometers (double)
    */
    double getDistance(double lat1, double lon1, double lat2, double lon2);

    /**
    * Function that returns the nearest stop to the coordinates given
    * @param lat latitude of the point
    * @param lon longitude of the point
    * @return pair where where the first element is the stop's code and the second one is the index of that stop in the stops vector
    */
    pair<string, int> getNearestStop(double lat, double lon);

    /**
    * Function that returns all the stops that are closer to X metres to a given point
    * @param lat the latitude of the starting point
    * @param lon the longitude of the starting point
    * @param x the maximum distance
    * @return a list with pairs where the first element is the stop code and the second is the corresponding index in the stops vector
    */
    list<pair<string, int>> getAllStopsCloserToXMetres(double lat, double lon, double x);

    /**
    * Function that calculates the total distance travelled in the course received as parameter
    * @param l list of integers that represent the course to be analyzed
    * @return total distance travelled in the course
    */
    double getTotalDistance(int src, const list<Edge>& l);

    /**
    * Function that calculates the total number of times the user has changed lines
    * @param l list of integers that represent the course to be analyzed
    * @return total number of times the user has changed lines
    */
    int getLineChange(list<Edge> l);

    /**
    * Function that calculates the total number of times the user has changed zones
    * @param l list of integers that represent the course to be analyzed
    * @return the total number of times the user has changed zones
    */
    int getTotalZoneChanges(int src, list<Edge> l);

    /**
    * Function that reads the existing lines that connect the different bus stops
    * @param path string that contains the path to the file with the corresponding route of that line
    */
    void addEdges(const string &path);

    /**
     * Function that adds all the links between stops that are materialized by foot
     */
    void addOnFootEdges();

    /**
     * Checks if the string code can be used
     * @param code code to be checked
     * @return true if the code isn't in the forbiddenStops list
     */
    bool canUse(string code);
public:

    Application(string stopsPath, string linesPath, double distance, list<string> forbiddenStops, list<string> forbiddenLines, char nightOrDay);

    /**
     * Function that fills the stops vector with all the stops present in the file
     */
    void readStops();

    /**
    * Function that reads the lines from the lines.csv file
    */
    void readEdges();

    /**
    * Function that returns the amount of connected components of the graph
    * @return amount (integer) of connected components of the graph
    */
    int getConnectedComponents();

    const vector<Stop> &getStops() const;

    /**
    * Function that calculates the course (startingPoint -> targetPoint) that minimizes the distance travelled
    * @param lat1 the latitude of the starting bus stop
    * @param lat1 the longitude of the starting bus stop
    * @param lat2 the latitude of the target point
    * @param lon2 the longitude of the target point
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumDistance(double lat1, double lon1, double lat2, double lon2);

    /**
    * Function that calculates the course (stop1 -> targetPoint) that minimizes the distance travelled
    * @param stop1 the starting bus stop
    * @param lat2 the latitude of the target point
    * @param lon2 the longitude of the target point
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumDistance(const string& stop1, double lat2, double lon2);

    /**
    * Function that calculates the course (startingPoint -> stop2) that minimizes the distance travelled
    * @param lat1 the latitude of the starting point
    * @param lon1 the longitude of the starting point
    * @param stop2 the destination bus stop
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumDistance(double lat1, double lon1, const string& stop2);

    /**
    * Function that calculates the course (stop1 -> stop2) that minimizes the distance travelled
    * @param stop1 the starting bus stop
    * @param stop2 the destination bus stop
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumDistance(const string& stop1, const string& stop2);

    /**
    * Function that calculates the course (startingPoint -> targetStop) that minimizes the amount of stops used
    * @param lat1 the latitude of the starting point
    * @param lon1 the longitude of the starting point
    * @param stop2 destination stop
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumStops(double lat1, double lon1, double lat2, double lon2);

    /**
    * Function that calculates the course (stop1 -> targetPoint) that minimizes the amount of stops used
    * @param stop1 the starting bus stop
    * @param lat2 the latitude of the target point
    * @param lon2 the longitude of the target point
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumStops(const string& stop1, double lat2, double lon2);

    /**
    * Function that calculates the course (startingPoint -> stop2) that minimizes the amount of stops used
    * @param lat1 the latitude of the starting point
    * @param lon1 the longitude of the starting point
    * @param stop2 the destination bus stop
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumStops(double lat1, double lon1, const string& stop2);

    /**
    * Function that calculates the course (stop1 -> targetStop2) that minimizes the amount of stops used
    * @param stop1 source stop
    * @param stop2 destination stop
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumStops(const string& stop1, const string& stop2);

    /**
    * Function that calculates the course (startingPoint -> targetPoint) that minimizes the times the user changes lines
    * @param lat1 latitude of the startingPoint
    * @param lon1 longitude of the startingPoint
    * @param lat2 latitude of the targetPoint
    * @param lon2 longitude of the targetPoint
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumLines(double lat1, double lon1, double lat2, double lon2);

    /**
    * Function that calculates the course (stop1 -> targetPoint) that minimizes the times the user changes lines
    * @param stop1 code of the source stop
    * @param lat2 latitude of the targetPoint
    * @param lon2 longitude of the targetPoint
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumLines(const string& stop1, double lat2, double lon2);

    /**
    * Function that calculates the course (startingPoint -> stop2) that minimizes the times the user changes lines
    * @param lat1 latitude of the targetPoint
    * @param lon1 longitude of the targetPoint
    * @param stop2 code of the destination stop
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumLines(double lat1, double lon1, string stop2);

    /**
    * Function that calculates the course (stop1 -> stop2) that minimizes the times the user changes lines
    * @param stop1 the starting bus stop
    * @param stop2 the destination bus stop
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumLines(const string& stop1, const string& stop2);

    /**
    * Function that calculates the course (startingPoint -> stop2) that minimizes the amount of zones used
    * @param lat1 the latitude of the starting point
    * @param lon1 the longitude of the starting point
    * @param stop2 the destination bus stop
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumZones(double lat1, double lon1, double lat2, double lon2);

    /**
    * Function that calculates the course (startingPoint -> stop2) that minimizes the amount of zones used
    * @param stop1 the starting bus stop
    * @param lat2 the latitude of the target point
    * @param lon2 the longitude of the target point
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumZones(const string& stop1, double lat2, double lon2);

    /**
    * Function that calculates the course (stop1 -> stop2) that minimizes the zones used
    * @param lat1 the latitude of the starting point
    * @param lon1 the longitude of the starting point
    * @param lat2 the latitude of the target point
    * @param lon2 the longitude of the target point
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumZones(double lat1, double lon1, const string& stop2);

    /**
    * Function that calculates the course (stop1 -> stop2) that minimizes the zones used
    * @param stop1 the starting bus stop
    * @param stop2 the destination bus stop
    * @return a pair where the first element is the index of the starting node and the second is a list with the edges used in the calculated path
    */
    pair<int, list<Edge>> courseWithMinimumZones(const string& stop1, const string& stop2);

    /**
     * Calculates the MST of the entire graph
     * @param stop the code of the starting stop
     * @return a pair where the first element is the total distance of the MST and the second is the list of indexes of the bus stops that are included in the MST (by order)
     */
    pair<double, vector<int>> MST(const string& stop);
};
#endif //PROJETO2_APPLICATION_H