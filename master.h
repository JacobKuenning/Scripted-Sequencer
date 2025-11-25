#pragma once
#include "rtmidi/RtMidi.h"
#include "globals.h"
#include "color.h"
#include "variable.h"
#include <vector>
#include <string>
#include <thread>
#include <mutex>

class sequencer;
class script;

enum lineType : int{
    MESSAGE_LINE,
    FUNCTION_LINE,
    VARIABLE_LINE,
    SECTION_LINE
};

class master{
public:
    std::vector<std::thread> threads;
    std::vector<sequencer*> seqs;
    script* scr;
    RtMidiOut* midiout;
    std::vector<variable*> variables;

    bool done = false;

    int defBPM = 100;
    int defSubdivisions = 1;
    bool killMidiOnQuit = true;
    bool useDefChannel = false;
    int defChannel = 0;

    color functionColor = MAGENTA;
    color messageColor = BLUE;
    color sectionColor = CYAN;
    color variableColor = GREEN;

    backgroundcolor functionBackground = BG_NONE;
    backgroundcolor messageBackground = BG_NONE;
    backgroundcolor sectionBackground = BG_NONE;
    backgroundcolor variableBackground = BG_NONE;

    color threadColors[6] = {RED,YELLOW,GREEN,BLUE,MAGENTA,CYAN};

    master(script* s);
    int getNextID();
    void readConfig();
    void parseConfigLine(std::string file);
    void input();
    void branch(int i);
    void setRawMode(bool enable);
    void killAllMidi();
    void printLine(int pCounter, std::string l, lineType ltype, int seqID);
    void createVariable(std::string n, std::vector<std::string> v);
};
