#include "variable.h"
#include <iostream>

variable::variable(std::string n, std::vector<std::string> args){
    name = n;
    setValues(args);
}

std::string variable::getCurrValue(){
    std::string value = values[vCounter];

    // wrapping around if needed
    int vSize = values.size() - 1;
    int distance = std::abs(vIncrement);
    int dir = 0;
    if (vIncrement < 0) dir = -1;
    else if (vIncrement > 0) dir = 1;

    for (int i = 0; i < distance; i++)
    {
        vCounter += dir;

        if (vCounter > vSize)
            vCounter = 0;
        else if (vCounter < 0)
            vCounter = vSize;
    }
    
    

    return value;
}

void variable::setValues(std::vector<std::string> vs){
    vCounter = 0;
    values.clear();

    for (std::string s : vs){
        values.push_back(s);
    }
}