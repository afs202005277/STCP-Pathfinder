#include "Stop.h"

Stop::Stop(const string &code, const string &name, const string &zone, double latitude, double longitude) : code(code),
                                                                                                            name(name),
                                                                                                            zone(zone),
                                                                                                            latitude(
                                                                                                                    latitude),
                                                                                                            longitude(
                                                                                                                    longitude) {}

const string &Stop::getCode() const {
    return code;
}

const string &Stop::getZone() const {
    return zone;
}

Stop::Stop() {}

double Stop::getLatitude() const {
    return latitude;
}

double Stop::getLongitude() const {
    return longitude;
}
