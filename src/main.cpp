#include <iostream>
#include "Application.h"

int main() {
    Application application("../dataset/stops.csv", "../dataset/lines.csv", 50);
    auto stops = application.getStops();
    /*auto teste = application.courseWithMinimumStops(41.188138102636366, -8.63988573136558, 41.17639492070776, -8.692438308431848);
    for (auto inner:teste){
        for (auto elem:inner){
            cout << stops[elem].getCode() << endl;
        }
    }*/
    /*auto teste = application.courseWithMinimumStops("STA1", "FGTR1");
    for (auto elem:teste) {
        cout << stops[elem].getCode() << endl;
    }*/
    //cout << teste.front().size() << endl;
    //auto teste = application.courseWithMinimumDistance(41.184369951579704, -8.641757713581917, 41.174167013902576, -8.689407024963856);

    auto teste = application.courseWithMinimumLines(41.184369951579704, -8.641757713581917, 41.174167013902576, -8.689407024963856);
    for (auto elem:teste) {
        cout << stops[elem].getCode() << endl;
    }
}