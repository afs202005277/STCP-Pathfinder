#include <iostream>
#include <iomanip>
#include "Application.h"

string toUpper(string s) {
    for (char & i : s){
        i = toupper(i);
    }
    return s;
}

bool isAllDigit(const string& s) {
    for (auto c:s)
        if (!isdigit(c))
            return false;
    return true;
}

int readWalkingDistance() {
    string walkingDistance;
    cout << "Please input the distance (in meters) you are willing to walk on foot: ";
    do {
        getline(cin, walkingDistance);
        if (isAllDigit(walkingDistance))
            return stoi(walkingDistance);
        cout << "Invalid distance! Please try again: ";
    }while(true);
}

char readNightOrDay() {
    string nightOrDay;
    cout << "Night or day trips? (N/D) ";
    do {
        getline(cin, nightOrDay);
        if (nightOrDay.size() == 1 && (toupper(nightOrDay[0]) == 'D' || toupper(nightOrDay[0]) == 'N'))
            return nightOrDay[0];
        cout << "Invalid input! Please try again: ";
    }while(true);
}

list<string> readForbiddenStops(){
    string stopCode;
    list<string> tmp;
    cout << "Please input the code of the bus stop you want to ignore (you can leave this field blank or press 'q' to move to the next step): ";
    while(getline(cin, stopCode)){
        if (stopCode.empty() || stopCode == "q")
            break;
        else
            tmp.push_back(toUpper(stopCode));
        cout << "Next stop: ";
    }
    return tmp;
}

list<string> readForbiddenLines() {
    string lineCode;
    list<string> tmp;
    cout << "Now you can choose the lines you don't want to use (you can leave this field blank or press 'q' to move to the next step). \nInput them in the format [CODE]_[DIR]: ";
    do {
        getline(cin, lineCode);
        if (lineCode.empty() || lineCode[0] == 'q')
            break;
        if (lineCode.find('_') != string::npos) {
            tmp.push_back(lineCode);
            cout << "Next line: ";
        }
        else
            cout << "Invalid input! Please try again: ";
    }while(true);
    return tmp;
}

void readMSTChoice(Application &application, vector<Stop> &stops) {
    cout << "Do you want to calculate the MST of the graph? (y/n) ";
    bool flag;
    do {
        string choice;
        getline(cin, choice);
        if (choice.size() == 1) {
            if (toupper(choice[0]) == 'Y') {
                flag = true;
                break;
            }
            else if (toupper(choice[0]) == 'N') {
                flag = false;
                break;
            }
        }
        cout << "Invalid input! Please try again: ";
    }while(true);
    pair<double, vector<int>> tmp;
    if (flag) {
        string startingNode;
        cout << "What is the starting node? (stop code) ";
        getline(cin, startingNode);
        tmp = application.MST(startingNode);
        cout << "The cost of the MST is: " << tmp.first << " kilometers." << endl;
        cout << "The nodes reached by the algorithm were: " << endl;
        for (int i=0;i<tmp.second.size();i++){
            cout << setw(5) << stops[tmp.second[i]].getCode() << "  ";
            if ((i+1) % 10 == 0)
                cout << endl;
        }
        cout << endl;
    }
}

int main() {
    cout << "Welcome to the STCP routing system." << endl;
    int walkingDistance = readWalkingDistance();
    char nightOrDay = readNightOrDay();
    list<string> forbiddenStops = readForbiddenStops();
    list<string> forbiddenLines = readForbiddenLines();

    Application application("../dataset/stops.csv", "../dataset/lines.csv", walkingDistance, forbiddenStops, forbiddenLines, nightOrDay);
    auto stops = application.getStops();

    readMSTChoice(application, stops);

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
            src = toUpper(tmp1);
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
            dest = toUpper(tmp1);
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
            if (srcUsingCode && destUsingCode)
                res = application.courseWithMinimumLines(src, dest);
            else if ((!srcUsingCode) && destUsingCode)
                res = application.courseWithMinimumLines(lat1, lon1, dest);
            else if (srcUsingCode && (!destUsingCode))
                res = application.courseWithMinimumLines(src, lat2, lon2);
            else if (!srcUsingCode && !destUsingCode)
                res = application.courseWithMinimumLines(lat1, lon1, lat2, lon2);
        }
        if (res.second.empty())
            cout << "There isn't a path between the stops provided." << endl;
        else {
            cout << stops[res.first].getCode();
            for (auto edge: res.second) {
                if (!edge.line.empty())
                    cout << " - " << edge.line.substr(0, edge.line.find('_')) << endl << stops[edge.dest].getCode();
                else
                    cout << " - " << "ON FOOT" << endl << stops[edge.dest].getCode();
            }
        }
        cout << endl;
        cout << "Do you want to calculate another route? (y/n)" << endl;
        getline(cin, tmp1);
        if (toupper(tmp1[0]) != 'Y')
            break;
    }
    return 0;
}