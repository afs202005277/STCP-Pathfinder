#include <iostream>
#include "Application.h"

int main() {
    Application application("../dataset/stops.csv", "../dataset/lines.csv", 0);
    //auto teste = application.courseWithMinimumStops(41.188138102636366, -8.63988573136558, 41.17639492070776, -8.692438308431848);
    auto teste = application.courseWithMinimumStops("AV2", "PCID4");
    cout << application.getConnectedComponents() << endl;
}
