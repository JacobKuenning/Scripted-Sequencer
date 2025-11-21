#include "variable.h"
#include <iostream>

variable::variable(std::string n, std::vector<std::string> args){
    name = n;
    setValues(args);
}

std::string variable::getCurrValue(){
    std::string value = values[vCounter];

    // wrap around if needed
    for (int i = 0; i < vIncrement; i++){
        if (vCounter + 1 > values.size()-1){
            vCounter = 0;
        }
        else { // otherwise don't
            vCounter++;
        }
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