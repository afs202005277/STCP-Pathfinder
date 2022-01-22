#include <iostream>
#include "Application.h"

int main() {
    Application application("../dataset/stops.csv", "../dataset/lines.csv", 600);
    auto stops = application.getStops();
    auto teste = application.courseWithMinimumStops(41.188138102636366, -8.63988573136558, 41.17639492070776, -8.692438308431848);
    for (auto inner:teste){
        for (auto elem:inner){
            cout << stops[elem].getCode() << endl;
        }
    }
    /*auto teste = application.courseWithMinimumStops("STA1", "FGTR1");
    for (auto elem:teste) {
        cout << stops[elem].getCode() << endl;
    }*/
    //cout << teste.front().size() << endl;
}