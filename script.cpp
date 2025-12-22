#include "script.h"
#include "lineutils.h"
#include "errors.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

script::script(std::string fileName){
    std::ifstream scriptFile(fileName);
    std::string str;

    if (!std::filesystem::exists(fileName))
        error("File \"%s\" does not exists.", fileName.c_str());

    while (std::getline(scriptFile, str)){
        lines.push_back(str);
    }

    if (lines.size() == 0){
        error("File \"%s\" has no lines.", fileName.c_str());
    }

    cleanScript();
    findSections();

    return;
}

script::~script(){
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

int script::strToLineNumber(std::string s, int linenum){
    if (isInt(s)){
        return std::stoi(s) - 1;
    }

    if (isValidSection(s)){
        return sections[s] - 1; //-1 so that the section name is visible
    }

    char suffix = s[0];
    if (suffix == 'r'){ // relative position
        std::string sub = s.substr(1,s.length()-1);
        if (isInt(sub)){
            return std::stoi(sub) + linenum - 1;
        }
    }
    return linenum;
}


bool script::isValidSection(std::string s){
    if (sections.find(s) != sections.end())
        return true;
    else return false;
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