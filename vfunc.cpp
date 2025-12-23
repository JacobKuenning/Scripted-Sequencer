#include "master.h"
#include "lineutils.h"
#include "errors.h"

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
    int argc = args.size();
    if (argcError("v_find_n_rep", argc, 3, 4)) return;

    std::string exc;
    std::string f = args[1];
    std::string r = args[2];
    if (argc == 4)
        exc = args[3]; 

    variableMx.lock();
    variable* v = varByName(args[0]);
    if (varError(v, args[0])) return;

    for (int i = 0; i < v->values.size(); i++){
        std::string oldValue = v->values[i];
        std::string newValue = findAndReplace(oldValue,f,r,exc);
        v->values[i] = newValue;
    }
    variableMx.unlock();
}

void master::varSetIncrement(std::vector<std::string> args){
    int argc = args.size();
    if (argcError("v_set_inc", argc, 2)) return;

    variableMx.lock();
    std::string var = args[0];

    if (!isInt(args[1])){
        warning("Invalid argument.");
        variableMx.unlock();
        return;
    }

    int newInc = std::stoi(args[1]);

    variable* v = varByName(args[0]);
    if (varError(v, args[0])) return;

    v->vIncrement = newInc;
    variableMx.unlock();
}

void master::varSetCounter(std::vector<std::string> args){
    int argc = args.size();
    if (argcError("v_set_counter", argc, 2)) return;

    variableMx.lock();

    if (!isInt(args[1])){
        warning("Invalid argument.");
        variableMx.unlock();
        return;
    }

    variable* v = varByName(args[0]);
    if (varError(v, args[0])) return;

    int newCounter = std::stoi(args[1]);
    int vSize = v->values.size() -1;
    // if the number passed is outside of the bounds
    if (newCounter > vSize) newCounter = v->values.size() - 1;
    if (newCounter < 0) newCounter = 0;
    v->vCounter = newCounter;
    variableMx.unlock(); 
}

void master::varAddElement(std::vector<std::string> args){
    int argc = args.size();
    if (argcError("v_add_element", argc, 2)) return;

    variableMx.lock();
    variable* v = varByName(args[0]);
    if (varError(v, args[0])) return;

    v->values.push_back(args[1]);
    variableMx.unlock();
}

void master::varInsertElement(std::vector<std::string> args){
    int argc = args.size();
    if (argcError("v_insert_element", argc, 3)) return;

    variableMx.lock();
    variable* v = varByName(args[0]);
    if (varError(v, args[0])) return;

    int index = std::stoi(args[2]);
    if (index < 0) index = 0;
    if (index > v->values.size()) index = v->values.size();
    v->values.insert(v->values.begin() + index, args[1]);
    variableMx.unlock();
}

void master::varRemoveElement(std::vector<std::string> args){
    int argc = args.size();
    if (argcError("v_remove_element", argc, 2)) return;

    variableMx.lock();
    variable* v = varByName(args[0]);
    if (varError(v, args[0])) return;

    int index = std::stoi(args[1]);
    if (index < 0 || index > v->values.size() - 1){
        variableMx.unlock(); 
        return;
    } 

    if (index < v->vCounter) v->vCounter--;

    v->values.erase(v->values.begin() + index);
    variableMx.unlock();
}

void master::varRemoveElementsByValue(std::vector<std::string> args){
    int argc = args.size();
    if (argcError("v_remove_element", argc, 2)) return;

    variableMx.lock();
    variable* v = varByName(args[0]);
    if (varError(v, args[0])) return;

    std::string find = args[1];

    for (int i = 0; i < v->values.size(); i++){
        if (v->values[i] == find){
            if (i < v->vCounter) v->vCounter--;
            v->values.erase(v->values.begin() + i);
        }
    }
    variableMx.unlock();
}
