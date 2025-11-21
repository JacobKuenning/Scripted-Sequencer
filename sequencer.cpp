#include "sequencer.h"
#include "script.h"
#include "message.h"
#include "conv.h"
#include "rtmidi/RtMidi.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <random>

sequencer::sequencer(script* scr){
    midiout = new RtMidiOut();
    midiout->openPort(0);

    readConfig();

    clock = 60000 / (bpm*subdivisons);

    s = scr;
    srand(time(0));

    while (pCounter < s->sLength){
        parseLine(pCounter);
        if (!incStack.empty()){
            pCounter += incStack.back();
            incStack.pop_back();
        }
        else{
            pCounter += increment;
        }   
    }

    return;
}

void sequencer::play(message m){
    std::vector<unsigned char> msg(3);
    if (m.status == 0x80){ // if we have a channel wide note off
        for (int i: playedNotes[m.channel]){ 
            msg[0] = m.channel | 0x80;
            msg[1] = i;
            msg[2] = 0;
            midiout->sendMessage(&msg);
        }
        playedNotes[m.channel].clear();
    } else {
        msg[0] = m.channel | m.status;
        msg[1] = m.note;
        msg[2] = m.velocity;
        midiout->sendMessage(&msg);
        // check if note is already in the played notes vector, add it to vector if not
        for (int note : playedNotes[m.channel]){
            if (m.note == note){
                return; 
            }
        }
        playedNotes[m.channel].push_back(m.note);
    } 
}

void sequencer::wait(){
    std::this_thread::sleep_for(std::chrono::milliseconds(clock));
    return;
}

void sequencer::parseLine(int l){
    std::string line = s->getLine(l);

    if (line == "@END"){
        if (!addrStack.empty()){
            pCounter = addrStack.back();
            addrStack.pop_back();
        }
    }

    char c = line[0];

    if (c == '-'){
        setVariable(line);
        printLine(line, variableColor);
    }
    else { 
        line = replaceVariables(line);
        line = resolveSets(line);
        c = line[0];

        if (c == '|'){ // if the line is a message
            printLine(line, messageColor);
            parseMessage(line);
            wait();
        }     
        else if (c == '~'){ // function
            printLine(line, functionColor);
            parseFunction(line);
        }
        else if (c == 'x'){ // empty line
            printLine(line, messageColor);   
            wait();
        }
        else if (c == '@'){
            printLine(line,sectionColor);
        }
    }
}

//finds all lists, replaces it with a random choice of things within the set
std::string sequencer::resolveSets(std::string line){
    std::vector<std::string> sets;
    size_t start = line.find_first_of("{");
    size_t end = line.find_first_of("}");

    if (start == std::string::npos || end == std::string::npos)
        return line;

    while (end != std::string::npos)
    {
        std::string set = line.substr(start, end-start+1);
        sets.push_back(set);
        start = end;
        start = line.find_first_of("{",start);
        end = line.find_first_of("}",start);
    }
    for (std::string s: sets){
        std::string copy = s;
        copy = copy.substr(1,copy.length()-2);
        std::vector<std::string> args = splitIntoArguments(copy);
        std::string choice = args[rand() % args.size()];
        int fpos = line.find(s); 
        line.erase(fpos,s.size()); // erase set from string 
        line.insert(fpos,choice); // replace it with chosen argument
    }
    return line;
}

void sequencer::parseMessage(std::string l){
    std::string line = l;
    std::vector<std::string> messagesText = splitIntoMessages(l);
    std::vector<message> messages;
    for (std::string mess : messagesText){
        std::vector<std::string> args = splitIntoArguments(mess);
        message m(args);
        messages.push_back(m);
    }

    for (message m: messages){
        play(m);
    }
}

void sequencer::parseFunction(std::string l){
    std::string line = l.erase(0,1);
    int startParen = line.find_first_of("(");
    int endParen = line.find_first_of(")");
    std::string funcName = line.substr(0, startParen);
    std::string argText = line.substr(startParen+1, endParen-startParen-1);
    std::vector<std::string> args = splitIntoArguments(argText);
    if (funcName == "SET_BPM"){
        setBPM(args);
    } else if (funcName == "SET_SUBDIVISIONS"){
        setSubdivisions(args);
    } else if (funcName == "SKIP"){
        skipLines(args);
    } else if (funcName == "PLAY"){
        playSection(args);
    } else if (funcName == "WAIT_MS"){
        waitMilliseconds(args);
    } else if (funcName == "SET_dINCREMENT"){
        changeIncrement(args);
    } else if (funcName == "REVERSE"){
        reverse(args);
    } else if (funcName == "GO_TO"){
        goToLine(args);
    } else if (funcName == "FINISH"){
        finish(args);
    }
}

void sequencer::setVariable(std::string l){
    std::string line = l;
    line = line.substr(1, line.size()-1);
    int spacePos = line.find_first_of("=");
    std::string name = line.substr(0,spacePos);
    std::string value = line.substr(spacePos + 1, line.size()- spacePos);
    variables[name] = value;
}

void sequencer::setBPM(std::vector<std::string> args){
    bpm = std::stoi(args[0]);
    clock = 60000 / (bpm * subdivisons);
}

void sequencer::setSubdivisions(std::vector<std::string> args){
    subdivisons = std::stoi(args[0]);
    clock = 60000 / (bpm * subdivisons);
}

void sequencer::skipLines(std::vector<std::string> args){
    int by = std::stoi(args[0]) - 1;
    pCounter += by;
}

void sequencer::goToLine(std::vector<std::string> args){
    int l = std::stoi(args[0]);
    pCounter = l - 1;
}

void sequencer::finish(std::vector<std::string> args){
    pCounter = s->sLength;
}

void sequencer::waitMilliseconds(std::vector<std::string> args){
    int ms = stoi(args[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void sequencer::playSection(std::vector<std::string> args){
    addrStack.push_back(pCounter);
    pCounter = s->sections[args[0]] -1;
}

void sequencer::changeIncrement(std::vector<std::string> args){
    int argSize = args.size();

    if (argSize == 1){ // change increment
        int change = std::stoi(args[0]);
        increment = change;  
    } 
}

void sequencer::reverse(std::vector<std::string> args){
    int amount = std::stoi(args[0]);
    for (int i = 0; i < amount; i++)
    {
        incStack.push_back(-1);
    }
    
}

std::string sequencer::replaceVariables(std::string line){
    if (line.find_first_of("$") == std::string::npos)
        return line;

    std::string copy = line;

    while (copy.find_first_of("$") != std::string::npos){
        for (const auto& variable : variables) {
            int vPos = copy.find("$"+variable.first);
            while (vPos != std::string::npos)
            {
                copy.erase(vPos, variable.first.length()+1);
                copy.insert(vPos, variable.second);
                vPos = copy.find("$"+variable.first);
            }  
        }
    }
    return copy;
}

std::vector<std::string> sequencer::splitIntoMessages(std::string l){
    std::string line = l;
    std::vector<std::string> messagesText;
    
    size_t start = 1;
    size_t end = line.find_first_of("|", start);

    while (end != std::string::npos){
        messagesText.push_back(line.substr(start, end - start));
        start = end + 1;
        end = line.find_first_of("|",start);
    }
    
    return messagesText;
}

std::vector<std::string> sequencer::splitIntoArguments(std::string m){
    std::vector<std::string> argumentsText;

    size_t start = 0;
    size_t end = 0;

    while (end != std::string::npos){
        if (m[start] == ',') // for when it just added a quoted arg, the next char will be a comma, so skip it
            start += 1;
        if (start >= m.size()) // when last arg is quoted, it will start you at the end of the string, adding an empty arg
            break;
        if (m[start] == '\"'){
            end = m.find_first_of("\"", start+1);
            argumentsText.push_back(m.substr(start+1, end - start - 1));
        }
        else {
            end = m.find_first_of(",", start);
            argumentsText.push_back(m.substr(start, end - start));
        }
        start = end + 1;
    }

    return weightArguments(argumentsText);
}

std::vector<std::string> sequencer::weightArguments(std::vector<std::string> args){
    std::vector<std::string> weightedArgs;
    for (std::string a : args){
        size_t wPos = a.find_first_of(":");
        if (wPos == std::string::npos){
            weightedArgs.push_back(a);
            continue;
        }   
        
        std::string arg = a.substr(0,wPos);
        int weight = std::stoi(a.substr(wPos+1,a.size()-wPos-1));

        for (int i = 0; i < weight; i++)
        {
            weightedArgs.push_back(arg);
        }
    }
    return weightedArgs;
}

void sequencer::printLine(std::string l, color c){
    int colorInt = static_cast<int>(c);
    std::string wrap = "\033[" + std::to_string(colorInt) + 'm' + l + "\033[0m";
    std::string spaces;
    if (pCounter >= 0 && pCounter < 10){ // keeps all of the output in line
        spaces = "   ";
    } else if (pCounter >= 10 && pCounter < 100){
        spaces = "  ";
    } else {
        spaces = " ";
    }

    std::cout << "[" << pCounter << "] " << spaces << wrap << std::endl;
}

void sequencer::error(std::string message, int l){
    std::cout << "Line (" << l << "): " << message << std::endl;
    std::cout << "--- " << s->getLine(l) << " ---" << std::endl;
}

void sequencer::debug(std::string m){
    std::cout << m << std::endl;
}
