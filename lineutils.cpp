#include "sequencer.h"
#include "lineutils.h"
#include <cctype>
#include <algorithm>

std::vector<std::string> splitIntoMessages(std::string l){
    std::string line = l;
    std::vector<std::string> messagesText;
    
    size_t start = 1;
    size_t end = line.find_first_of("|", start);

    while (end != std::string::npos){
        std::string m = line.substr(start, end-start);
        if (!m.empty()) messagesText.push_back(m);
        start = end + 1;
        end = line.find_first_of("|",start);
    }
    
    return messagesText;
}

std::vector<std::string> splitIntoArguments(std::string m){
    std::vector<std::string> argumentsText;

    size_t start = 0;
    size_t end = 0;

    while (end != std::string::npos){
        if (m[start] == ',') // for when it just added a quoted arg, the next char will be a comma, so skip it
            start += 1;
        if (start >= m.size()) // when last arg is quoted, it will start you at the end of the string, adding an empty arg
            break;
        if (m[start] == '\"'){
            end = m.find_first_of("\"", start+1);
            argumentsText.push_back(m.substr(start+1, end - start - 1));
        }
        else {
            end = m.find_first_of(",", start);
            argumentsText.push_back(m.substr(start, end - start));
        }
        start = end + 1;
    }

    return weightArguments(argumentsText);
}

std::vector<std::string> weightArguments(std::vector<std::string> args){
    std::vector<std::string> weightedArgs;
    for (std::string a : args){
        size_t wPos = a.find_first_of(":");
        if (wPos == std::string::npos){
            weightedArgs.push_back(a);
            continue;
        }   
        
        std::string arg = a.substr(0,wPos);
        int weight = std::stoi(a.substr(wPos+1,a.size()-wPos-1));

        for (int i = 0; i < weight; i++)
        {
            weightedArgs.push_back(arg);
        }
    }
    return weightedArgs;
}

std::string resolveRandom(std::string line){
    std::vector<std::string> sets;
    size_t start = line.find_first_of("{");
    size_t end = line.find_first_of("}");

    if (start == std::string::npos || end == std::string::npos)
        return line;

    while (end != std::string::npos)
    {
        std::string set = line.substr(start, end-start+1);
        sets.push_back(set);
        start = end;
        start = line.find_first_of("{",start);
        end = line.find_first_of("}",start);
    }
    for (std::string s: sets){
        std::string copy = s;
        copy = copy.substr(1,copy.length()-2);
        std::vector<std::string> args = splitIntoArguments(copy);
        std::string choice = args[rand() % args.size()];
        int fpos = line.find(s); 
        line.erase(fpos,s.size()); // erase set from string 
        line.insert(fpos,choice); // replace it with chosen argument
    }
    return line;
}

std::string lowercase(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// replaces all f with r, except those included in exc
std::string findAndReplace(std::string str, std::string f, std::string r, std::string exc){
    // replace all of the exc with a newline, placeholder
    size_t pos = 0;
    while ((pos = str.find(exc, pos)) != std::string::npos){
        str.erase(pos,exc.length());
        str.insert(pos,"\n");
        pos += 1;
    }

    pos = 0;
    while ((pos = str.find(f,pos)) != std::string::npos){
        str.erase(pos,f.length());
        str.insert(pos,r);
        pos += r.length();
    }

    pos = 0;
    while ((pos = str.find("\n",pos)) != std::string::npos){
        str.erase(pos,1);
        str.insert(pos,exc);
        pos += exc.length();
    }

    return str;
}