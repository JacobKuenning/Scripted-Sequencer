#pragma once

#include <string>
#include <vector>

class variable{
    public:
    std::string name;
    std::vector<std::string> values;
    int vCounter = 0;
    int vIncrement = 1;

    variable(std::string n, std::vector<std::string> args);
    std::string getCurrValue();
    void setValues(std::vector<std::string> vs);
};