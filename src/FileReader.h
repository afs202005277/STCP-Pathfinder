//
// Created by andre on 19/01/2022.
//

#ifndef PROJETO2_FILEREADER_H
#define PROJETO2_FILEREADER_H
using namespace std;
#include <vector>

#include "Stop.h"

class FileReader {
private:
    vector<Stop> stops;
    string filePath;

public:
    FileReader(const string &filePath);
    vector<Stop> read();
};


#endif //PROJETO2_FILEREADER_H
