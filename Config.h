#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>

class Config{
    std::string file;
    std::map<std::string, std::string> content;
public:
    Config(std::string);
    
    const char* operator[](const std::string&);
};
extern Config CONFIG;
#endif