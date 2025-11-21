#include <string>
#include <vector>
#include <map>
#include "rtmidi/RtMidi.h"

class script;
class message;

enum function : int{
    VARIABLE
};

enum color : int{
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
};

class sequencer{
    public:
    sequencer(script* s);
    void play(message m);
    script* s;
    RtMidiOut *midiout;

    int pCounter = 0;
    int increment = 1;
    std::vector<int> incStack;
    int clock = 0;
    int subdivisons = 1;
    int bpm = 100;
    std::vector<int> addrStack;
    std::map<std::string,std::string> variables;

    color functionColor = MAGENTA;
    color messageColor = BLUE;
    color sectionColor = CYAN;
    color variableColor = GREEN;

    void parseLine(int l);
    void parseMessage(std::string l);
    void parseFunction(std::string l);
    std::string resolveSets(std::string l);

    void setVariable(std::string line);
    void setBPM(std::vector<std::string> args);
    void setSubdivisions(std::vector<std::string> args);
    void wait();
    void waitMilliseconds(std::vector<std::string> args);

    void skipLines(std::vector<std::string> args);
    void playSection(std::vector<std::string> args);
    void goToLine(std::vector<std::string> args);
    void finish(std::vector<std::string> args);

    void changeIncrement(std::vector<std::string> args);
    void reverse(std::vector<std::string> args);

    std::string replaceVariables(std::string line);
    std::vector<std::string> splitIntoMessages(std::string l);
    std::vector<std::string> splitIntoArguments(std::string m);
    std::vector<std::string> weightArguments(std::vector<std::string> args);

    void printLine(std::string l, color c);
    void error(std::string message, int l);
    void debug(std::string m);
    std::vector<int> playedNotes[16];
};


