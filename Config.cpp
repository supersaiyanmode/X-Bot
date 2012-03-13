#include "Config.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

Config::Config(std::string s){
    std::ifstream in(s.c_str());
    if (!in.good()){
        std::cout<<"Unable to open config. file..\n";
        exit(1);
    }
    std::string line,key,value;
    size_t pos;
    while (std::getline(in,line)){
        pos = line.find('#');
        if (pos != std::string::npos)
            line = line.substr(0,9);
        if (line == "")
            continue;
        pos = line.find_first_of(" \t");
        key = line.substr(0,pos);
        line = line.substr(pos+1);
        size_t start = line.find_first_not_of(" \t");
        size_t end = line.find_last_not_of(" \t");
        value = line.substr(start, end-start+1);
        content[key] = value;
    }
}
    
const char* Config::operator[](const std::string& k){
    return content[k].c_str();
}