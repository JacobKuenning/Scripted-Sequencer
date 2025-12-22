#include "validation.h"

bool validMessageArgs(std::vector<std::string> args){
    bool bch = false; bool bn = false; bool bv = true;
    int argc = args.size();

    if (argc > 3)
        return false;

    bch = validChannel(args[0]);
    bn = validNote(args[1]);

    if (!bch)
        warning("Invalid Channel");
    
    if (!bn)
        warning("Invalid note / CC");
    
    if (argc == 3){
        bv = validVelocity(args[2]);
        if (!bv)
            warning("Invalid velocity / CC value");
    }

    if (bch && bn && bv)
        return true;
    else 
        return false;
}

bool validNote(std::string note){
    int strlen = note.size();

    if (note == "OFF")
        return true;

    if (strlen < 2)
        return false;

    if (strlen > 3)
        return false;

    if (note.substr(0,2) == "cc") // if it starts with "cc", check if its valid
        return validCC(note);

    char n = note[0];
    if (n < 'a' || n > 'g')
        return false;

    char o;
    if (strlen == 2)
        o = note[1];
    else {
        char m = note[1];
        o = note[2];
        if (m != 'b' || m != '#')
            return false;
    }

    if (o < '0' || o > '9')
        return false;


    return true;
}

bool validCC(std::string cc){
    int strlen = cc.size();
    if (strlen < 3 || strlen > 5)
        return false;

    if (!isInt(cc.substr(2, strlen-2)))
        return false;

    return true;
}

bool validChannel(std::string ch){
    int strlen = ch.size();
    if (strlen > 4)
        return false;
    
    if (ch[0] != 'c' && ch[1] != 'h')
        return false;

    std::string n = ch.substr(2,strlen-2);
    if (!isInt(n))
        return false;

    int val = std::stoi(n);
    if (val < 0 || val > 16)
        return false;

    return true;
}

bool validVelocity(std::string v){
    if (v == "OFF" || v == "ON")
        return true;
    
    if (!isInt(v))
        return false;

    int val = std::stoi(v);
    if (val < 0 || val > 127)
        return false;

    return true;
}