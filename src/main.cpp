#include <iostream>
#include "FileReader.h"

int main() {
    FileReader fileReader("../dataset/stops.csv");
    vector<Stop> stops = fileReader.readStops();

}
