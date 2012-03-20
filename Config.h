#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>


/*
 * A simple configuration utility
 * Using a std::map to store key-value read from the file.
 */

class Config{
    std::string file;
    std::map<std::string, std::string> content;
public:
    Config(std::string);
    void destroy();

    //[] overloaded to apply directly to map - content
    std::string& operator[](const std::string&);
};
extern Config CONFIG;
#endif