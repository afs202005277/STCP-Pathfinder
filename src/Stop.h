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

    Stop();

    const string &getZone() const;

    const string &getCode() const;

    double getLatitude() const;

    double getLongitude() const;
};


#endif //PROJETO2_STOP_H
