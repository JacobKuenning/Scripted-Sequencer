#include "master.h"
#include "sequencer.h"
#include "script.h"
#include "message.h"
#include "variable.h"
#include "conv.h"
#include "globals.h"
#include "lineutils.h"
#include "validation.h"
#include "errors.h"
#include "rtmidi/RtMidi.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <random>
#include <functional>

// for input
#include <termios.h>
#include <unistd.h>

sequencer::sequencer(master* mas, script* scr, int start, RtMidiOut* mout, int seqID){
    m = mas;
    pCounter = start;
    midiout = mout;

    ID = seqID;
    bpm = m->defBPM;
    subdivisons = m->defSubdivisions;
    useDefChannel = m->useDefChannel;
    defaultChannel = m->defChannel;

    clock = 60000 / (bpm*subdivisons);
    s = scr;
    srand(time(0));
}

void sequencer::run(){
    while (running && pCounter < s->sLength){
        parseLine(pCounter);
        if (!incStack.empty()){
            pCounter += incStack.back();
            incStack.pop_back();
        } else {
            pCounter += increment;
        }
        while (paused){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    running = false;
}

// deconstructor
sequencer::~sequencer() {
}

void sequencer::play(message m){
    midiMx.lock();
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
        msg[0] = m.channel + m.status;
        msg[1] = m.note;
        msg[2] = m.velocity;
        midiout->sendMessage(&msg);
        if (m.status == 0xB0) {
            midiMx.unlock();    
            return; // if cc message, no need to go further
        }

        // check if note is already in the played notes vector, add it to vector if not
        for (int note : playedNotes[m.channel]){
            if (m.note == note){
                midiMx.unlock();
                return; 
            }
        }
        playedNotes[m.channel].push_back(m.note);
    } 
    midiMx.unlock();
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
        m->setVariable(line);
        m->printLine(pCounter, line, VARIABLE_LINE, ID, name);
    }
    else { 
        line = replaceVariables(line);
        line = resolveRandom(line);
        c = line[0];

        if (c == '|'){ // if the line is a message
            m->printLine(pCounter, line, MESSAGE_LINE, ID, name);
            parseMessage(line);
            wait();
        }     
        else if (c == '#'){ // function
            m->printLine(pCounter, line, FUNCTION_LINE, ID, name);
            parseFunction(line);
        }
        else if (c == 'x'){ // empty line
            m->printLine(pCounter, line, MESSAGE_LINE, ID, name);   
            wait();
        }
        else if (c == '@'){ 
            m->printLine(pCounter, line, SECTION_LINE, ID, name);
        }
    }
}

void sequencer::parseMessage(std::string l){
    std::string line = l;
    std::vector<std::string> messagesText = splitIntoMessages(l);
    std::vector<message> messages;
    for (std::string mess : messagesText){
        std::vector<std::string> args = splitIntoArguments(mess);
        if (args[0].find("ch") == std::string::npos && useDefChannel){ // if no channel specified in first arg
            std::string chStr = "ch" + std::to_string(defaultChannel);
            args.insert(args.begin(),chStr); // insert the default channel
        }
        if (!validMessageArgs(args)){ // don't create message if arguments are invalid
            warning(" ^^^ Invalid arguments.");
            return;
        }
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
    funcName = lowercase(funcName);
    std::string argText = line.substr(startParen+1, endParen-startParen-1);
    std::vector<std::string> args = splitIntoArguments(argText);
    if (funcName == "set_bpm"){
        setBPM(args);
    } else if (funcName == "set_steps_per_beat"){
        setSubdivisions(args);
    } else if (funcName == "play"){
        playSection(args);
    } else if (funcName == "new_seq"){
        createSequencer(args);
    } else if (funcName == "end_seq"){
        end(args);
    } else if (funcName == "pause_seq"){
        pause(args);
    } else if (funcName == "resume_seq"){
        m->resumeSequencer(args);
    } else if (funcName == "wait_ms"){
        waitMilliseconds(args);
    } else if (funcName == "set_inc"){
        changeIncrement(args);
    } else if (funcName == "reverse"){
        reverse(args);
    } else if (funcName == "go_to"){
        goToLine(args);
    } else if (funcName == "v_find_n_rep"){
        m->varFindAndReplace(args);
    } else if (funcName == "v_set_inc"){
        m->varSetIncrement(args);
    } else if (funcName == "v_set_counter"){
        m->varSetCounter(args);
    } else if (funcName == "v_add_element"){
        m->varAddElement(args);
    } else if (funcName == "v_insert_element"){
        m->varInsertElement(args);
    } else if (funcName == "v_remove_element"){
        m->varRemoveElement(args);
    } else if (funcName == "v_remove_elements_by_value"){
        m->varRemoveElementsByValue(args);
    } else if (funcName == "start_cc"){
        startCC(args);
    } else if (funcName == "none"){
        
    }
}

void sequencer::setBPM(std::vector<std::string> args){
    bpm = std::stoi(args[0]);
    clock = 60000 / (bpm * subdivisons);
}

void sequencer::setSubdivisions(std::vector<std::string> args){
    subdivisons = std::stoi(args[0]);
    clock = 60000 / (bpm * subdivisons);
}

void sequencer::goToLine(std::vector<std::string> args){
    int linen = s->strToLineNumber(args[0], pCounter);
    pCounter = linen;
}

void sequencer::waitMilliseconds(std::vector<std::string> args){
    int ms = stoi(args[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void sequencer::playSection(std::vector<std::string> args){
    addrStack.push_back(pCounter);
    pCounter = s->sections[args[0]] -1;
}

void sequencer::createSequencer(std::vector<std::string> args){
    int argc = args.size();
    if (argc == 1)
        m->branch("", s->strToLineNumber(args[0], pCounter) +1);
    else if (argc == 2)
        m->branch(args[0], s->strToLineNumber(args[1], pCounter) +1);
}

void sequencer::end(std::vector<std::string> args){
    int argc = args.size();
    if (argc == 0){
        running = false;
        return;
    }
    else 
        m->stopSequencer(args);
}

void sequencer::pause(std::vector<std::string> args){
    int argc = args.size();
    if (argc == 0){
        paused = true;
        return;
    }
    else    
        m->stopSequencer(args);
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

    variableMx.lock();
    while (copy.find_first_of("$") != std::string::npos){
        for (variable* v : m->variables) {
            std::string variableRef = "$" + v->name;
            int vPos = copy.find(variableRef);
            while (vPos != std::string::npos)
            {
                copy.erase(vPos, v->name.length()+1);
                copy.insert(vPos, v->getCurrValue());
                vPos = copy.find("$"+v->name);
            }  
        }
    }
    variableMx.unlock();
    return copy;
}

void sequencer::startCC(std::vector<std::string> args){
    int ch = std::stoi(args[0]);
    int cc = std::stoi(args[1]);
    int s = std::stoi(args[2]);
    int e = std::stoi(args[3]);
    float t = std::stof(args[4]);
    m->createCCLerper(ch,cc,s,e,t);
}

void sequencer::error(std::string message, int l){
    std::cout << "Line (" << l << "): " << message << std::endl;
    std::cout << "--- " << s->getLine(l) << " ---" << std::endl;
}

void sequencer::debug(std::string m){
    std::cout << m << std::endl;
}

// turns off canonical mode, to detect key presses immediately
void sequencer::setRawMode(bool enable) {
    static termios oldt;
    termios newt;

    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);  // no buffering, no echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}
