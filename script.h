#pragma once
#include <string>
#include <vector>

class script{
public:
    std::vector<std::string> lines;
    script(std::string fileName);
    void cleanScript();
    void printLines();
    void findAndReplace(std::string f, std::string r);
};