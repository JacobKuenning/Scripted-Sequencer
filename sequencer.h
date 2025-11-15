#include <string>
#include <vector>

class script;

class sequencer{
    public:
    sequencer(script* s);
    script* s;
    void wait(int s);
    bool parseLine(int l);
    std::vector<std::string> splitIntoMessages(int l);
    //std::vector<std::string> splitIntoArguments(std::string m);
    int countMessages(int l);
    void printLine(int l);
    void error(std::string message, int l);
    int pCounter = 0;
};