//
// Created by andre on 19/01/2022.
//

#ifndef PROJETO2_STOP_H
#define PROJETO2_STOP_H

#include <string>

using namespace std;

class Stop {
private:
    string code, name, zone;
    double latitude, longitude;

public:
    Stop(const string &code, const string &name, const string &zone, double latitude, double longitude);
};


#endif //PROJETO2_STOP_H
