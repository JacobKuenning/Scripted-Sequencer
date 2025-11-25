#pragma once

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <atomic>
#include <functional>
#include "rtmidi/RtMidi.h"
#include "color.h"


class script;
class message;
class variable;
class master;

enum function : int{
    VARIABLE
};

class sequencer{
    public:
    sequencer(master* m, script* s, int start, RtMidiOut* mout, int seqID);
    void run();
    ~sequencer();
    std::atomic<bool> running = true;
    bool mainSequencer = false;
    void play(message m);
    void setRawMode(bool enable);
    script* s;
    master* m;
    int ID = 0;
    RtMidiOut *midiout;
    std::vector<std::string> outputVector;


    int pCounter = 0;
    int increment = 1;
    std::vector<int> incStack;
    int clock = 0;
    int subdivisons = 1;
    int bpm = 100;
    std::vector<int> addrStack;

    bool useDefChannel = false;
    int defaultChannel = 1;
    bool killMidiOnQuit = true;

    void parseLine(int l);
    void parseMessage(std::string l);
    void parseFunction(std::string l);

    void setVariable(std::string line);
    void setBPM(std::vector<std::string> args);
    void setSubdivisions(std::vector<std::string> args);
    void wait();
    void waitMilliseconds(std::vector<std::string> args);

    void skipLines(std::vector<std::string> args);
    void playSection(std::vector<std::string> args);
    void createSequencer(std::vector<std::string> args);
    void goToLine(std::vector<std::string> args);
    void finish(std::vector<std::string> args);

    void changeIncrement(std::vector<std::string> args);
    void reverse(std::vector<std::string> args);

    std::string replaceVariables(std::string line);

    void error(std::string message, int l);
    void debug(std::string m);
    std::vector<int> playedNotes[16];
};


