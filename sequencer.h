#include <string>
#include <vector>
#include "rtmidi/RtMidi.h"

class script;
class message;

class sequencer{
    public:
    sequencer(script* s);
    void play(message m);
    script* s;
    RtMidiOut *midiout;
    void wait(int s);
    bool parseLine(int l);
    std::vector<std::string> splitIntoMessages(int l);
    std::vector<std::string> splitIntoArguments(std::string m);
    int countMessages(int l);
    void printLine(int l);
    void error(std::string message, int l);
    int pCounter = 0;
    std::vector<int> playedNotes[16];
};

