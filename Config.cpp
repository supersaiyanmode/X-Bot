#include "Config.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

Config::Config(std::string s):file(s){
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
    in.close();
}
void Config::destroy(){
    std::ofstream out(file.c_str());
    if (!out.good()){
        std::cout<<"Unable to open config. file..\n";
        exit(1);
    }
    for (std::map<std::string, std::string>::iterator it=content.begin();
                    it!=content.end(); it++){
        out<<it->first<<"\t\t\t"<<it->second<<std::endl;
    }
    out.close();
}

std::string& Config::operator[](const std::string& k){
    return content[k];
}