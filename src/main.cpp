#include <iostream>
#include "Application.h"
#include <vector>

int main() {
    Application application("../dataset/stops.csv", "../dataset/lines.csv", 0);
    auto stops = application.getStops();
    auto translator = application.getStopToInt();

    cout << "Welcome to the STCP routing system." << endl;
    vector<int> stack;
    while (true) {
        cout << "Please input the coordinates (or the stop code) of the departure location." << endl;
        string tmp1, tmp2, src, dest;
        bool srcUsingCode = false, destUsingCode = false;
        double lat1, lon1, lat2, lon2;
        try {
            getline(cin, tmp1);
            lat1 = stod(tmp1);
            for (auto letter:tmp1)
                if (letter != '.' && !isdigit(letter))
                    throw exception();
            getline(cin, tmp1);
            lon1 = stod(tmp1);
        } catch (...) {
            src = tmp1;
            srcUsingCode = true;
        }
        cout << "Please input the coordinates (or the stop code) of the arrival." << endl;
        try {
            getline(cin, tmp1);
            lat2 = stod(tmp1);
            for (auto letter:tmp1)
                if (letter != '.' && !isdigit(letter))
                    throw exception();
            getline(cin, tmp1);
            lon2 = stod(tmp1);
        } catch (...) {
            dest = tmp1;
            destUsingCode = true;
        }
        int option;
        cout << "There are several routing methods." << endl;
        cout << "1- The route with the minimum number of stops." << endl;
        cout << "2- The shortest route." << endl;
        cout << "3- The cheapest route." << endl;
        cout << "4- The route with the minimum number of lines used." << endl;
        cout << "Please input the number of the method you want to use:  ";
        cin >> option;
        cin.ignore();

        pair<int, list<Edge>> res;
        if (option == 1) {
            if (srcUsingCode && destUsingCode)
                res = application.courseWithMinimumStops(src, dest);
            else if ((!srcUsingCode) && destUsingCode)
                res = application.courseWithMinimumStops(lat1, lon1, dest);
            else if (srcUsingCode && (!destUsingCode))
                res = application.courseWithMinimumStops(src, lat2, lon2);
            else if (!srcUsingCode && !destUsingCode)
                res = application.courseWithMinimumStops(lat1, lon1, lat2, lon2);
        } else if (option == 2) {
            if (srcUsingCode && destUsingCode)
                res = application.courseWithMinimumDistance(src, dest);
            else if ((!srcUsingCode) && destUsingCode)
                res = application.courseWithMinimumDistance(lat1, lon1, dest);
            else if (srcUsingCode && (!destUsingCode))
                res = application.courseWithMinimumDistance(src, lat2, lon2);
            else if (!srcUsingCode && !destUsingCode)
                res = application.courseWithMinimumDistance(lat1, lon1, lat2, lon2);
        } else if (option == 3) {
            if (srcUsingCode && destUsingCode)
                res = application.courseWithMinimumZones(src, dest);
            else if ((!srcUsingCode) && destUsingCode)
                res = application.courseWithMinimumZones(lat1, lon1, dest);
            else if (srcUsingCode && (!destUsingCode))
                res = application.courseWithMinimumZones(src, lat2, lon2);
            else if (!srcUsingCode && !destUsingCode)
                res = application.courseWithMinimumZones(lat1, lon1, lat2, lon2);
        } else if (option == 4) {
            if (srcUsingCode && destUsingCode) {
                auto tmp = application.courseWithMinimumLines(src, dest, stack);
                for (auto i:tmp.second)
                    cout << stops[i].getCode() << endl;
                return 0;
            }
            else if ((!srcUsingCode) && destUsingCode)
                //res = application.courseWithMinimumLines(lat1, lon1, dest);
                cout << "Not Implemented" << endl;
            else if (srcUsingCode && (!destUsingCode))
                //res = application.courseWithMinimumLines(src, lat2, lon2);
                cout << "Not Implemented" << endl;
            else if (!srcUsingCode && !destUsingCode)
                res = application.courseWithMinimumLines(lat1, lon1, lat2, lon2);
        }
        cout << stops[res.first].getCode();
        for (auto edge:res.second){
            cout << " - " << edge.line.substr(0, edge.line.find('_')) << endl << stops[edge.dest].getCode();
        }
        cout << endl;
        cout << "Do you want to calculate another route? (y/n)" << endl;
        getline(cin, tmp1);
        if (tmp1 == "n")
            break;
    }
    return 0;
}