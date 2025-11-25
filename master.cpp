#include "globals.h"
#include "master.h"
#include "script.h"
#include "sequencer.h"
#include "message.h"
#include "conv.h"
#include "rtmidi/RtMidi.h"
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fstream>
#include <algorithm>

void master::readConfig(){
    std::ifstream cFile("config.txt");

    std::string line;
    while(getline(cFile,line)){
        parseConfigLine(line);
    }
 
    cFile.close();
}

void master::parseConfigLine(std::string line){
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
        defBPM = std::stoi(value);
    else if (var == "defaultSubdivisions")
        defSubdivisions = std::stoi(value);
    else if (var== "defaultChannel"){
        if (value != "NONE"){
            useDefChannel = true;
            defChannel = std::stoi(value);
        }
        else
            useDefChannel = false;
    } else if (var == "killMidiOnQuit"){
        if (value == "TRUE")
            killMidiOnQuit = true;
        else
            killMidiOnQuit = false;
    }
}


void master::printLine(int pCounter, std::string l, lineType ltype, int seqID){

    color fg; backgroundcolor bg;
    switch(ltype){
        case MESSAGE_LINE:
            fg = messageColor; bg = messageBackground;
            break;
        case FUNCTION_LINE:
            fg = functionColor; bg = functionBackground;
            break;    
        case VARIABLE_LINE:
            fg = variableColor; bg = variableBackground;
            break;
        case SECTION_LINE:
            fg = sectionColor; bg = sectionBackground;
            break;
    }

    int colorInt = static_cast<int>(fg);
    std::string colorString = "\033[" + std::to_string(colorInt) + 'm';
    std::string bgString;
    if (bg != BG_NONE){
        int bgInt = static_cast<int>(bg);
        bgString = "\033[" + std::to_string(bgInt) + 'm';
    }
    std::string endFormat = "\033[0m";
    std::string wrap = colorString + bgString + l + endFormat;
    std::string spaces;
    if (pCounter >= 0 && pCounter < 10){ // keeps all of the output in line
        spaces = "   ";
    } else if (pCounter >= 10 && pCounter < 100){
        spaces = "  ";
    } else {
        spaces = " ";
    }

    std::string output = "[" + std::to_string(pCounter) + "] "  + spaces + wrap + "\n";
    outputMx.lock();
    std::string threadC = "\033[" + std::to_string(threadColors[seqID-1 % 6]) + 'm';
    std::string threadS = threadC + '[' + std::to_string(seqID) + ']' + endFormat;
    std::cout << threadS + output;
    outputMx.unlock();
}   

void master::input(){
    while(!done){
        int c = getchar();
        if (c=='q'){
            vectorMx.lock();
            for (sequencer* n : seqs){
                n->running = false;
            }
            done = true;
            vectorMx.unlock();
        }
    }
}

// create new sequencer, start it at line i, it will run until it reaches an @ END
void master::branch(int i){
    sequencer* seq = new sequencer(this, scr, i, midiout, getNextID());
    vectorMx.lock();
    seqs.push_back(seq);
    std::thread seqThread;
    seqThread = std::thread(&sequencer::run, seq);
    threads.push_back(std::move(seqThread));
    vectorMx.unlock();
    return;
}

void master::setRawMode(bool enable) { 
    static termios oldt; 
    termios newt; 
    if (enable) { 
        tcgetattr(STDIN_FILENO, &oldt); 
        newt = oldt; 
        newt.c_lflag &= ~(ICANON | ECHO); // no buffering, no echo tcsetattr(STDIN_FILENO, TCSANOW, &newt); } else { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); } }
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); 
    } else { 
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); 
    } 
}

void master::killAllMidi(){
    std::vector<message> offMessages;

    for (int i = 0; i < 16; i++){
        for (int n = 0; n < 108; n++)
        {
            std::vector<unsigned char> msg(3);
            msg[0] = i | 0x80;
            msg[1] = n;
            msg[2] = 0;
            midiout->sendMessage(&msg);
        }
    }
}

// find lowest possible unique ID
int master::getNextID(){
    std::vector<int> IDs = {0};
    for (sequencer* seq : seqs){
        IDs.push_back(seq->ID);
    }

    std::sort(IDs.begin(), IDs.end());
    std::vector<int> possibleIDs;

    int next = 0;
    for (int id : IDs) {
        if (id == next) // move until you've found a gap
            next++;
        else if (id > next) // gap found
            break;
    }
    return next;
}

master::master(script* s){
    scr = s;
    midiout = new RtMidiOut;
    midiout->openPort(0);
    readConfig();
    setRawMode(true);

    std::thread inputThread = std::thread(&master::input, this);

    sequencer* seq = new sequencer(this,s, 0, midiout, getNextID());
    seqs.push_back(seq);
    std::thread seqThread;
    seqThread = std::thread(&sequencer::run, seq);
    threads.push_back(std::move(seqThread));

    // main thread periodically loops through, joins finished threads and frees memory
    while(!threads.empty()){
        vectorMx.lock();
        for (int i= 0; i < seqs.size(); i++){
            if (seqs[i]->running == false){
                if (threads[i].joinable()){
                    threads[i].join();
                    delete seqs[i];
                    seqs.erase(seqs.begin()+i);
                    threads.erase(threads.begin() + i);
                }
            }
        }
        vectorMx.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    inputThread.join();
    if (killMidiOnQuit) killAllMidi();

    delete midiout;
    setRawMode(false);
    return;
}