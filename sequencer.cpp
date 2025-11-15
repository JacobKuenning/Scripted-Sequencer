#include "sequencer.h"
#include "script.h"
#include "rtmidi/RtMidi.h"

#include <chrono>
#include <thread>
#include <iostream>

sequencer::sequencer(script* scr){
    RtMidiOut *midiout = new RtMidiOut();
    midiout->openPort(0);

    s = scr;

    while (pCounter < s->sLength){
        if (parseLine(pCounter))
            //printLine(pCounter);
        wait(1);
        pCounter++;
    }

    return;
}

void sequencer::wait(int s){
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return;
}

bool sequencer::parseLine(int l){
    std::string line = s->getLine(l);
    if (line[0] != '|'){
        error("Syntax Error - Message Not Parsed", l);
        return false;
    }
    if (line[line.size()-1] != '|'){
        error("Syntax Error - Message Not Parsed", l);
        return false;
    }

    std::vector<std::string> messages = splitIntoMessages(l);
    for (std::string m : messages){
        splitIntoArguments(m);
    }

    return true;
}

std::vector<std::string> sequencer::splitIntoMessages(int l){
    std::string line = s->getLine(l);
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
    size_t end = m.find_first_of(",");

    while (end != std::string::npos){
        argumentsText.push_back(m.substr(start, end - start));
        start = end + 1;
        end = m.find_first_of(",", start);
    }
    argumentsText.push_back(m.substr(start, end - start));

    for (std::string arg : argumentsText){
        std::cout << arg << std::endl;
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