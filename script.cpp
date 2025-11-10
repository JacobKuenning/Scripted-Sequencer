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
    printLines();

    return;
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

    // removing all spaces
    for (int j = 0; j < lines.size(); j++){
        std::string l = lines[j];
        int i = 0;
        while (i < l.size()){
            if (l[i] == ' '){
                l.erase(i,1);
            }
            else 
                i++;
        }
        lines[j] = l;
    }
    return;
}

void script::printLines(){
    for (std::string l : lines){
        std::cout << l << std::endl;
    }
    return;
}