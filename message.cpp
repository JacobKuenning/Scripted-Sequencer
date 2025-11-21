#include "message.h"
#include "conv.h"
#include <iostream>

message::message(int ch, int n, int v){
    channel = ch;
    note = n;
    velocity = v;
}

message::message(std::vector<std::string> args){
    int argc = args.size();
    channel = channelToInt(args[0]);
    
    if (args[1] == "OFF"){ // channel wide off message |ch1,OFF|
        status = 0x80;
        note = 0;
        velocity = 0;
        return;
    }

    if (args[1].find("cc") != std::string::npos){ // cc message
        status = 0xB0;
        std::string ccStr = args[1];
        ccStr.erase(0,2);
        note = std::stoi(ccStr);
    } else {
        status = 0x90;
        note = noteToInt(args[1]);   
    }

    if (argc == 2){ // if there's no third argument, it's assumed that you want max velocity
        velocity = 127;
        return;
    } 

    if (args[2] == "OFF") // if message of form |ch1,g3,OFF|, a velocity of 0 will turn that note off
        velocity = 0;
    else
        velocity = std::stoi(args[2]);
}