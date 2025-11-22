#include "script.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

script::script(std::string fileName){
    std::ifstream scriptFile(fileName);
    std::string str;
    while (std::getline(scriptFile, str)){
        lines.push_back(str);
    }
    cleanScript();
    findSections();

    return;
}

script::~script(){
    std::cout << "SCRIPT DECONSTRUCTED" << std::endl;
}

void script::cleanScript(){
    // removing all comments
    for (int i = 0; i < lines.size(); i++){
        std::string l = lines[i];
        int commentPosition = l.find("//");
        if (commentPosition != std::string::npos){
            l.erase(commentPosition, l.size() - commentPosition);
            lines[i] = l;
        }
    }

    // removing all spaces and newlines
    for (int j = 0; j < lines.size(); j++){
        std::string l = lines[j];
        int i = 0;
        while (i < l.size()){
            if (l[i] == ' ' || l[i] == '\n'){
                l.erase(i,1);
            }
            else 
                i++;
        }
        lines[j] = l;
    }

    // removing all empty lines
    std::vector<std::string> copy;
    for (std::string l : lines){
        if (l.size() != 0){
            copy.push_back(l);
        }
    }
    lines = copy;
    sLength = lines.size();
    return;
}

void script::findSections(){
    for (int i = 0; i < sLength; i++){
        std::string l = lines[i];
        if (l[0] == '@'){
            if (l.substr(1,l.size()-1) != "END"){
                std::string name = l.substr(1,l.size()-1);
                sections[name] = i;
            }
        }
    }
}

void script::findAndReplace(std::string f, std::string r){
    for (int i = 0; i < lines.size(); i++){
        std::string l = lines[i];
        int fpos = l.find(f);
        if (fpos != std::string::npos){
            l.erase(fpos,f.size());
            l.insert(fpos,r);
            lines[i] = l;
        }
    }
}

void script::printLines(){
    for (std::string l : lines){
        std::cout << l << std::endl;
    }
    return;
}

std::string script::getLine(int n){
    return lines[n];
}