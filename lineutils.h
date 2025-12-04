#include <vector>
#include <string>

std::vector<std::string> splitIntoMessages(std::string l);
std::vector<std::string> splitIntoArguments(std::string m);
std::vector<std::string> weightArguments(std::vector<std::string> args);
std::string resolveRandom(std::string line);
std::string lowercase(std::string str);
std::string findAndReplace(std::string str, std::string f, std::string r, std::string exc);
bool isInt(std::string str);
bool textToBool(std::string str);
