#include "sequencer.h"
#include "script.h"
#include "message.h"
#include "strmidiconv.h"
#include "rtmidi/RtMidi.h"

#include <chrono>
#include <thread>
#include <iostream>

sequencer::sequencer(script* scr){
    midiout = new RtMidiOut();
    midiout->openPort(0);

    s = scr;

    while (pCounter < s->sLength){
        parseLine(pCounter);
        pCounter++;
    }

    return;
}

void sequencer::play(message m){
    if (m.status == 0x80){
        for (int i: playedNotes[m.channel]){
            std::vector<unsigned char> v;
            v.push_back(m.channel | 0x80);
            v.push_back(i);
            v.push_back(0);
            midiout->sendMessage(&v);
        }
        playedNotes[m.channel].clear();
    } else {
        std::vector<unsigned char> v;
        v.push_back(m.channel | m.status);
        v.push_back(m.note);
        v.push_back(m.velocity);
        midiout->sendMessage(&v);
        playedNotes[m.channel].push_back(m.note);
    }
}

void sequencer::wait(){
    std::this_thread::sleep_for(std::chrono::milliseconds(clock));
    return;
}

void sequencer::parseLine(int l){
    std::string line = s->getLine(l);
    line = replaceVariables(line);
    printLine(l);
    if (line[0] == '|'){
        parseMessage(line);
        wait();
    }     
    else if (line[0] == 'x')
        wait();
    else if (line[0] == '~')
        parseFunction(line);
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
    if (funcName == "VARIABLE"){
        setVariable(args);
    } else if (funcName == "BPM"){
        setBPM(args);
    } else if (funcName == "SUBDIVISIONS"){
        setSubdivisions(args);
    } else if (funcName == "CLOCK_MS"){
        setClock(args);
    }
}

void sequencer::setVariable(std::vector<std::string> args){
    variables[args[0]] = args[1];
}

void sequencer::setBPM(std::vector<std::string> args){
    bpm = std::stoi(args[0]);
    clock = 60000 / (bpm * subdivisons);
}

void sequencer::setClock(std::vector<std::string> args){
    clock = std::stoi(args[0]);
}


void sequencer::setSubdivisions(std::vector<std::string> args){
    subdivisons = std::stoi(args[0]);
    clock = 60000 / (bpm * subdivisons);
}

std::string sequencer::replaceVariables(std::string line){
    std::string copy = line;
    if (copy.find_first_of("$") == std::string::npos)
        return copy;

    for (const auto& variable : variables) {
        int vPos = copy.find("$"+variable.first);
        while (vPos != std::string::npos)
        {
            copy.erase(vPos, variable.first.length()+1);
            copy.insert(vPos, variable.second);
            vPos = copy.find("$"+variable.first);
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

    return argumentsText;
}



int sequencer::countMessages(int l){
    std::string line = s->getLine(l);
    int sum = 0;
    for (char c : line){
        if (c == '|')
            sum += 1;
    }
    return sum - 1;
}

void sequencer::printLine(int l){
    std::cout << s->getLine(l) << std::endl;
}

void sequencer::error(std::string message, int l){
    std::cout << "Line (" << l << "): " << message << std::endl;
    std::cout << "--- " << s->getLine(l) << " ---" << std::endl;
}