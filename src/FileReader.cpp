//
// Created by andre on 19/01/2022.
//

#include <fstream>
#include "FileReader.h"

FileReader::FileReader(const string &filePath) : filePath(filePath) {}

vector<string> getValues(string line){
    
}

vector<Stop> FileReader::read() {
    fstream fileStream(filePath);
    string line;
    while(getline(fileStream, line)){
        getValues(line);
    }
}
