#include "master.h"
#include "lineutils.h"

void master::setVariable(std::string l){
    std::string line = l;
    line = line.substr(1, line.size()-1);
    int spacePos = line.find_first_of("=");
    std::string name = line.substr(0,spacePos);
    std::string value = line.substr(spacePos + 1, line.size()- spacePos);
    std::vector<std::string> values;

    int bPos = value.find_first_of("[");
    if (bPos == std::string::npos){ // if there's no brackets
        values.push_back(value);
    } else {
        value = value.substr(1,value.size()-2);
        values = splitIntoArguments(value);
    }

    // if variable already exists
    variableMx.lock();
    variable *v = varByName(name);
    if (v != nullptr){
        v->setValues(values);
        variableMx.unlock();
        return;
    }
    
    // otherwise, create new value
    variable* var = new variable(name,values);
    variables.push_back(var);
    variableMx.unlock();
}

variable* master::varByName(std::string n){
    for (variable* v : variables){
        if (v->name == n)
            return v;
    }
    return nullptr;
}

void master::varFindAndReplace(std::vector<std::string> args){
    std::string f = args[1];
    std::string r = args[2];
    std::string exc = args[3];

    variableMx.lock();
    variable* v = varByName(args[0]);
    for (int i = 0; i < v->values.size(); i++){
        std::string oldValue = v->values[i];
        std::string newValue = findAndReplace(oldValue,f,r,exc);
        v->values[i] = newValue;
    }
    variableMx.unlock();
}

void master::varSetIncrement(std::vector<std::string> args){
    variableMx.lock();
    std::string var = args[0];
    int newInc = std::stoi(args[1]);

    variable* v = varByName(args[0]);
    v->vIncrement = newInc;
    variableMx.unlock();
}

void master::varSetCounter(std::vector<std::string> args){
    variableMx.lock();
    variable* v = varByName(args[0]);
    int newCounter = std::stoi(args[1]);
    int vSize = v->values.size() -1;
    // if the number passed is outside of the bounds
    if (newCounter > vSize) newCounter = v->values.size() - 1;
    if (newCounter < 0) newCounter = 0;
    v->vCounter = newCounter;
    variableMx.unlock(); 
}
