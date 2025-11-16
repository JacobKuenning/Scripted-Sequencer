#include <string>
#include <vector>
#include <map>
#include "rtmidi/RtMidi.h"

class script;
class message;

enum function : int{
    VARIABLE
};

class sequencer{
    public:
    sequencer(script* s);
    void play(message m);
    script* s;
    RtMidiOut *midiout;
    int clock = 600;
    int subdivisons = 4;
    int bpm = 100;
    std::map<std::string,std::string> variables;
    void wait();
    void parseLine(int l);
    void parseMessage(std::string l);
    void parseFunction(std::string l);

    void setVariable(std::vector<std::string> args);
    void setBPM(std::vector<std::string> args);
    void setSubdivisions(std::vector<std::string> args);
    void setClock(std::vector<std::string> args);

    std::string replaceVariables(std::string line);
    std::vector<std::string> splitIntoMessages(std::string l);
    std::vector<std::string> splitIntoArguments(std::string m);
    int countMessages(int l);
    void printLine(int l);
    void error(std::string message, int l);
    int pCounter = 0;
    std::vector<int> playedNotes[16];
};


