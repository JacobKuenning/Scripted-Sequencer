#pragma once
#include <string>
#include <vector>
#include <map>

class script{
public:
    std::vector<std::string> lines;
    int sLength = 0;
    std::map<std::string, int> sections;
    script(std::string fileName);
    void cleanScript();
    void printLines();
    void findSections();
    void findAndReplace(std::string f, std::string r);
    std::string getLine(int n);
};