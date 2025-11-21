#include "sequencer.h"
#include "conv.h"
#include <fstream>

void sequencer::readConfig(){
    std::ifstream cFile("config.txt");

    std::string line;
    while(getline(cFile,line)){
        parseConfigLine(line);
    }
 
    cFile.close();
}

void sequencer::parseConfigLine(std::string line){
    if (line.empty())
        return;

    std::string var;
    std::string value;

    size_t eqPos = line.find_first_of("=");
    var = line.substr(0,eqPos);
    value = line.substr(eqPos+1,line.size()-eqPos+1);

    // colors
    if (var == "functionColor")
        functionColor = stringToColor(value);
    else if (var == "messageColor")
        messageColor = stringToColor(value);
    else if (var == "sectionColor")
        sectionColor = stringToColor(value);
    else if (var == "variableColor")
        variableColor = stringToColor(value);

    // background colors
    else if (var == "functionBackground")
        functionBackground = stringToBGColor(value);
    else if (var == "messageBackground")
        messageBackground = stringToBGColor(value);
    else if (var == "sectionBackground")
        sectionBackground = stringToBGColor(value);
    else if (var == "variableBackground")
        variableBackground = stringToBGColor(value);

    // sequencer settings
    else if (var == "defaultBPM")
        bpm = std::stoi(value);
    else if (var == "defaultSubdivisions")
        subdivisons = std::stoi(value);
    else if (var== "defaultChannel"){
        if (value != "NONE"){
            useDefChannel = true;
            defaultChannel = std::stoi(value);
        }
        else
            useDefChannel = false;
    }
}
